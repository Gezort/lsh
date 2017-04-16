import numpy as np
import random


class LSHHashBase(object):
    def __init__(self, *args, **kwargs):
        raise NotImplementedError()

    def __call__(self, *args, **kwargs):
        raise NotImplementedError()


class LSHHashFamilyBase(object):
    def __init__(self, *args, **kwargs):
        raise NotImplementedError()

    def _get_r_1(self):
        raise NotImplementedError()

    def _get_r_2(self):
        raise NotImplementedError()

    def _get_p_1(self):
        raise NotImplementedError()

    def _get_p_2(self):
        raise NotImplementedError()

    def __call__(self):
        raise NotImplementedError()


class LSHHammingHash(LSHHashBase):
    def __init__(self, dimension, projection=None):
        self._dimension = dimension
        self._projection = projection if projection is not None else random.randint(0, self._dimension - 1)

    def __call__(self, p):
        """
        Computes hash function, prepends input with zeros if needed
            @param p - ndarray, binary vector
        """
        assert (len(p) <= self._dimension)
        j = self._projection - (self._dimension - len(p))
        if j < 0:
            return 0
        else:
            return p[j]


class LSHL1Hash(LSHHashBase):
    def __init__(self, dimension, max_coordinate, projection=None):
        self._dimension = dimension
        self._max_coordinate = max_coordinate
        if projection is not None:
            self._projection = projection
        else:
            self._projection = random.randint(0, self._dimension * self._max_coordinate - 1)

    def __call__(self, p):
        """
        Computes hash function, prepends input with zeros if needed
            @param p - ndarray, integer vector
        """
        assert (len(p) <= self._dimension)
        j = self._projection - (self._dimension - len(p)) * self._max_coordinate
        if j < 0:
            return 0
        else:
            i = (j + self._max_coordinate - 1) / self._max_coordinate
            j %= self._max_coordinate
            if p[i] >= j:
                return 1
            else:
                return 0


class LSHHammingHashFamily(LSHHashFamilyBase):
    def __init__(self, dimension, allowed_distance, margin):
        """
        hash function family, $H(r_1, r_2, p_1, p_2)$ see "Sim& search in high dimensions..." in 
            "Proc. of 25th VLDB conf", 1999
            @param dimension - dimension of space
            @param allowed_distance - max distance between "similar" points ($r_1$)
            @param margin - relative margin between "similar" and "distinct" points 
                ($\epsilon$ in article, $r_2 = r (1 + \epsilon)$)
        """
        self._allowed_distance = allowed_distance
        self._margin = margin
        self._dimension = dimension

    def _get_r_1(self):
        return self._allowed_distance

    def _get_r_2(self):
        return self._allowed_distance * (1 + self._margin)

    def _get_p_1(self):
        return 1.0 - self._allowed_distance / float(self._dimension)

    def _get_p_2(self):
        return 1 - self._allowed_distance * (1 + self._margin) / self._dimension

    p_1 = property(_get_p_1)
    p_2 = property(_get_p_2)
    r_1 = property(_get_r_1)
    r_2 = property(_get_r_2)

    def __call__(self):
        return LSHHammingHash(self._dimension)


class LSHL1HashFamily(LSHHashFamilyBase):
    def __init__(self, dimension, max_coordinate, allowed_distance, margin):
        """
        hash function family, $H(r_1, r_2, p_1, p_2)$ see "Sim& search in high dimensions..." in 
            "Proc. of 25th VLDB conf", 1999
            @param dimension - dimension of space
            @param max_coordinate - max coordinate value of points in P, ($C$)
            @param allowed_distance - max distance between "similar" points ($r_1$)
            @param margin - relative margin between "similar" and "distinct" points 
                ($\epsilon$ in article, $r_2 = r (1 + \epsilon)$)
        """
        self._allowed_distance = allowed_distance
        self._margin = margin
        self._dimension = dimension
        self._max_coordinate = max_coordinate

    def _get_r_1(self):
        return self._allowed_distance

    def _get_r_2(self):
        return self._allowed_distance * (1 + self._margin)

    def _get_p_1(self):
        return 1.0 - self._allowed_distance / float(self._dimension * self._max_coordinate)

    def _get_p_2(self):
        return 1 - self._allowed_distance * (1 + self._margin) / (self._dimension * self._max_coordinate)

    p_1 = property(_get_p_1)
    p_2 = property(_get_p_2)
    r_1 = property(_get_r_1)
    r_2 = property(_get_r_2)

    def __call__(self):
        return LSHL1Hash(self._dimension, self._max_coordinate)


class HashGroup(object):
    def __init__(self, hashes, bases=None, max_base=None):
        """
        @param hashes - list of hash functions each hash function assumed to return either 0 or 1
        bases or max_base - numbers to be multiplied on hash values
        """

        self._hashes = hashes
        self._bases = bases or np.random.randint(0, len(self._hashes) - 1, len(self._hashes))

    def __call__(self, p):
        """
        Computes all hash functions in defined in constructor
            returns integer number with binary representation equal to hashes calculation
        """

        hash_bits = np.array([h(p) for h in self._hashes])

        return np.sum(hash_bits * self._bases)


class LSHHammingStore(object):
    # TODO: Use better second level hashes
    def __init__(self, allowed_distance, margin, size, dimensions, max_coordinate=None, bucket_size=128,
                 memory_utilization=2, at_most_hashes_in_group=None, metric='hamming'):
        """ 
        @param metric: str, one of ['hamming', 'l1'] 
        """
        assert metric in ['hamming', 'l1'], 'Wrong metric'
        self._size = size
        self._bucket_size = bucket_size

        self._storage_size = memory_utilization * size / bucket_size
        self._storage = [list() for _ in range(self._storage_size)]

        if metric == 'hamming':
            self._hash_family = LSHHammingHashFamily(dimensions, allowed_distance, margin)
        elif metric == 'l1':
            assert max_coordinate is not None, 'Please set maximum coordinate in the set'
            self._hash_family = LSHL1HashFamily(dimensions, max_coordinate, allowed_distance, margin)

        # TODO: look for formulas for l1 metric
        # see P2 near Th 1 in article referenced above for what is rho and c
        rho = np.log(1.0 / self._hash_family.p_1) / np.log(1 / self._hash_family.p_2)
        self._c = 4.0

        self._hash_bits = int(np.ceil(np.log(bucket_size / float(size)) / np.log(self._hash_family.p_2)))
        self._hash_groups = int(np.ceil((size / float(bucket_size)) ** rho))

        if at_most_hashes_in_group is not None:
            assert self._hash_groups <= at_most_hashes_in_group

        def build_hash_group_of_size(size):
            return HashGroup([self._hash_family() for _ in range(size)])

        self._hashes = [build_hash_group_of_size(self._hash_bits) for _ in range(self._hash_groups)]

    def _get_hash_bits_count(self):
        return self._hash_bits

    def _get_hash_groups_count(self):
        return self._hash_groups

    hash_bits = property(_get_hash_bits_count)
    hash_groups = property(_get_hash_groups_count)

    def _put_in_bucket(self, p, bucket_id, strict=False):
        """
        Puts value in a bucket with given index
        @param strict - if True raises exception if bucket is full, else noop
        see note after algorithm definition in ref
        """

        bucket = self._storage[bucket_id]

        if len(bucket) < self._bucket_size:
            bucket.append(p)
            return True

        if strict:
            assert len(bucket) <= self._bucket_size
        else:
            return False

    def put(self, p):
        """
        Puts binary vector @param p into storage
        """
        for _hash in self._hashes:
            hash_value = _hash(p)
            bucket = hash_value % self._storage_size

            self._put_in_bucket(p, bucket)

    def _neighbours_candidates(self, q):
        candidates_to_find = int(np.ceil(self._c * self._hash_groups))

        candidates = []
        hash_group_index = 0
        while len(candidates) < candidates_to_find and hash_group_index < self._hash_groups:
            _hash = self._hashes[hash_group_index]
            hash_value = _hash(q)
            bucket = hash_value % self._storage_size
            candidates += self._storage[bucket]
            hash_group_index += 1

        return np.array(candidates[:candidates_to_find])

    @staticmethod
    def _distance(p, q):
        """
        Computes hamming distance between binary vectors p and q
        """

        return np.sum(p != q)

    def k_neighbours(self, q, k=1, return_distances=False):
        """
        Returns $\epsilon$, r (margin, allowed_distance) k-neighbours
        @param q - query vector
        """
        candidates = self._neighbours_candidates(q)
        distances = np.array([self._distance(q, x) for x in candidates])

        indexes_sorted = np.argsort(distances)

        if return_distances:
            return candidates[indexes_sorted, :][:k, :], distances[indexes_sorted][:k]
        else:
            return candidates[indexes_sorted, :][:k, :]


class ApproximateRNN(object):
    @staticmethod
    def _repeats_by_tolerance(tolerance):
        return np.ceil(1.0 / tolerance)

    def __init__(
            self,
            size, r, margin, bucket_size=128, memory_utilization=2,
            method_tolerance=None, lsh_stores=None,
            similar_point_same_hases_probability=None, hash_bits=None,
            ensure_enough_dimensions_for=None,
            at_most_hashes_in_group=None
    ):

        assert (similar_point_same_hases_probability is None and hash_bits is not None) or \
               (similar_point_same_hases_probability is not None and hash_bits is None)
        assert (method_tolerance is None and lsh_stores is not None) or \
               (method_tolerance is not None and lsh_stores is None)

        if hash_bits is not None:
            # see proof of Th. 1 in the ref
            p_2 = (bucket_size / float(size)) ** (1.0 / hash_bits)
            self._dimensions = int(np.ceil(r * (1 + margin) / (1 - p_2)))
        else:
            self._dimensions = int(np.ceil(r / (1 - similar_point_same_hases_probability)))

        if ensure_enough_dimensions_for is not None:
            assert self._dimensions >= ensure_enough_dimensions_for

        lsh_stores = lsh_stores if lsh_stores is not None else self._repeats_by_tolerance(tolerance)

        self._lsh_stores = [
            LSHHammingStore(
                r, margin, size, self._dimensions, bucket_size=bucket_size,
                memory_utilization=memory_utilization, at_most_hashes_in_group=at_most_hashes_in_group
            ) for _ in range(lsh_stores)
        ]

    def fit(self, X):
        for store in self._lsh_stores:
            for p in X:
                store.put(p)

    def k_neighbours(self, q, k=1, return_distances=False):
        unique_neighbours = set()
        neighbours = []
        distances = []

        for store in self._lsh_stores:
            ns, ds = store.k_neighbours(q, k=k, return_distances=True)
            for n, d in zip(ns, ds):
                if id(n) not in unique_neighbours:
                    unique_neighbours.add(id(n))
                    neighbours.append(n)
                    distances.append(d)

        distances = np.array(distances)
        neighbours = np.array(neighbours)

        order = np.argsort(distances)

        if return_distances:
            return np.copy(neighbours[order, :][:k, :]), np.copy(distances[order][:k])
        else:
            return np.copy(neighbours[order, :][:k, :])

    def _get_max_allowed_dimensions(self):
        return self._dimensions

    def _get_lsh_stores_count(self):
        return len(self._lsh_stores)

    def _get_hash_bits_count(self):
        return self._lsh_stores[0].hash_bits

    def _get_hash_groups_count(self):
        return self._lsh_stores[0].hash_groups

    dimensions = property(_get_max_allowed_dimensions)
    lsh_stores = property(_get_lsh_stores_count)
    hash_bits = property(_get_hash_bits_count)
    hash_groups = property(_get_hash_groups_count)

    # TODO make following code cleaner
    r_1 = property(lambda self: self._lsh_stores[0]._hash_family.r_1)
    r_2 = property(lambda self: self._lsh_stores[0]._hash_family.r_2)
    p_1 = property(lambda self: self._lsh_stores[0]._hash_family.p_1)
    p_2 = property(lambda self: self._lsh_stores[0]._hash_family.p_2)

    def __str__(self):
        # see the ref. for meaning of parameters
        return "RNN(d={}, repeats={}, l={}, k={}, r_1={}, r_2={}, p_1={}, p_2={})".format(
            self.dimensions,
            self.lsh_stores,
            self.hash_groups,
            self.hash_bits,
            self.r_1,
            self.r_2,
            self.p_1,
            self.p_2,
        )


if __name__ == '__main__':
    d = 32
    N = 512

    X = np.random.randint(0, 1, (N, d))

    rnn = ApproximateRNN(N, 2, 0.5, lsh_stores=1, hash_bits=16, ensure_enough_dimensions_for=d)
    print(str(rnn))
    rnn.fit(X)
