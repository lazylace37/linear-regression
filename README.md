# Linear Regression in C

Simple Linear Regression in C

## Build

```bash
make
```

## Usage

```
Usage: linreg [OPTION...] FILE
Linear Regression example

      --file=FILE            Input file
  -i, --iterations=iterations   Number of iterations (default 1000)
  -l, --learning-rate=learning_rate
                             The learning rate (default 0.001)
  -o, --output=OUTFILE       Output model file
      --skip-header          Skip CSV header (default false)
  -s, --separator=separator  CSV separator (default ';')
  -?, --help                 Give this help list
      --usage                Give a short usage message
```

for ex.

`./linreg /tmp/winequality-red.csv -o winequality-red-model --skip-header -i 3000 -l 0.01`

The program only accepts numeric CSVs, without quotes. The last column is the target.

---

For prediction, use:

```
Usage: predict [OPTION...] FILE
Linear Regression example

      --file=FILE            Input file
  -m, --model=FILE           Model file
      --skip-header          Skip CSV header (default false)
  -s, --separator=separator  CSV separator (default ';')
  -?, --help                 Give this help list
      --usage                Give a short usage message
```

for ex.

`./predict -m winequality-red-model /tmp/winequality-red.csv --skip-header`
