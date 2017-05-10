from unittest import TestCase
import numpy as np
from sklearn.neighbors import KNeighborsClassifier
import nose
from hamming import ApproximateRNN


class TestL1LSH(TestCase):

    def setUp(self):
        d = 5
        N = 512
        self._max_coord = 2

        self._X = np.random.randint(0, self._max_coord, (N, d))
        self._gt = KNeighborsClassifier(metric='hamming')

        self._gt.fit(self._X, np.zeros(self._X.shape[0]))

    def test_it_should_construct_lsh_store(self):
        rnn = ApproximateRNN(self._X.shape[0], 2, 0.5, lsh_stores=1, hash_bits=16,
                             ensure_enough_dimensions_for=self._X.shape[1],
                             metric='l1', max_coordinate=self._max_coord)

        self.assertIsNotNone(rnn, "Rnn object must be not None after construction")

    def test_it_should_fail_if_asked_to_store_too_large_objects(self):
        self.assertRaises(
            AssertionError,
            lambda *args, **kwargs: ApproximateRNN(*args, **kwargs),
            self._X.shape[0], 2, 0.5,
            lsh_stores=1,
            hash_bits=16,
            ensure_enough_dimensions_for=10 * self._X.shape[1],
            metric='l1',
            max_coordinate=self._max_coord
        )

    def test_it_should_find_something(self):
        rnn = ApproximateRNN(self._X.shape[0], 2, 0.5, lsh_stores=1, hash_bits=16,
                             ensure_enough_dimensions_for=self._X.shape[1],
                             metric='l1', max_coordinate=self._max_coord)

        rnn.fit(self._X)

        neighbours = rnn.k_neighbours(self._X[0], 10)

        self.assertIsNotNone(neighbours)
        self.assertTrue(np.all(neighbours[0] == self._X[0]), "Same element must always be found")
        self.assertTrue(len(neighbours) == 10, "Must found exactly same number of elements as asked to")

    def test_it_should_find_something(self):
        rnn = ApproximateRNN(self._X.shape[0], 5, 0.5, lsh_stores=20, hash_bits=16,
                             ensure_enough_dimensions_for=self._X.shape[1])

        rnn.fit(self._X)

        neighbours = rnn.k_neighbours(self._X[0], 10)

        self.assertIsNotNone(neighbours)
        self.assertTrue(np.all(neighbours[0] == self._X[0]), "Same element must always be found")

        closest = None
        for x in self._X:
            if not np.all(x == self._X[0]):
                if closest is None:
                    closest = x
                elif np.sum(np.abs(closest - self._X[0])) > np.sum(np.abs(x - self._X[0])):
                    closest = x
        self.assertTrue(np.all(neighbours[1] == closest), "Near must always be found")

if __name__ == '__main__':
    nose.main()
