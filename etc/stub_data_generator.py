import argparse
import sys
import numpy as np

def parse_arguments():
    parser = argparse.ArgumentParser(
        description='Generates random data for knn')
    parser.add_argument('N', type=int, help='Size of sample')
    parser.add_argument('d', type=int, help='Size of each sample')
    parser.add_argument('--out', default=sys.stdout, type=argparse.FileType('w'),
        help='output file (stdout by default)')

    args = parser.parse_args()
    return args

def main():
    args = parse_arguments()
    # p(1) = 0.01
    X = np.random.randint(101, size=(args.N, args.d))
    for row_id, x in enumerate(X):
        print >> args.out, row_id, " ".join([str(elt) for elt in np.where(x > 99)[0]])

if __name__ == '__main__':
    main()
