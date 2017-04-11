from unittest import TestCase
import numpy as np
from sklearn.neighbors import KNeighborsClassifier
import nose
from hamming import ApproximateRNN


class TestHammingLSH(TestCase):
    def _make_distinct(self, x, distinct_bits=1):
        d = np.copy(x)
        for i in range(distinct_bits):
            d[i] = 0 if d[i] == 1 else 1

        return d

    def setUp(self):
        d = 32
        N = 512

        self._X = np.random.randint(0, 2, (N, d))

        self._X[1] = self._make_distinct(self._X[0], 1)
        self._X[2] = self._make_distinct(self._X[0], 2)
        self._X[3] = self._make_distinct(self._X[0], 3)

        self._gt = KNeighborsClassifier(metric='hamming')

        self._gt.fit(self._X, np.zeros(self._X.shape[0]))

    def test_it_should_construct_lsh_store(self):
        rnn = ApproximateRNN(self._X.shape[0], 2, 0.5, lsh_stores=1, hash_bits=16,
                             ensure_enough_dimensions_for=self._X.shape[1])

        self.assertIsNotNone(rnn, "Rnn object must be not None after construction")

    def test_it_shoul_fail_if_asked_to_store_too_large_objects(self):
        self.assertRaises(
            AssertionError,
            lambda *args, **kwargs: ApproximateRNN(*args, **kwargs),
            self._X.shape[0], 2, 0.5,
            lsh_stores=1,
            hash_bits=16,
            ensure_enough_dimensions_for=10 * self._X.shape[1]
        )

    def test_it_should_find_something(self):
        rnn = ApproximateRNN(self._X.shape[0], 2, 0.5, lsh_stores=1, hash_bits=16,
                             ensure_enough_dimensions_for=self._X.shape[1])

        rnn.fit(self._X)

        neighbours = rnn.k_neighbours(self._X[0], 10)

        self.assertIsNotNone(neighbours)
        self.assertTrue(np.all(neighbours[0] == self._X[0]), "Same element must always be found")
        self.assertTrue(len(neighbours) == 10, "Must found exactly same number of elements as asked to")


if __name__ == '__main__':
    nose.main()
