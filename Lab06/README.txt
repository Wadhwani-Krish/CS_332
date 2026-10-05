# String Length Comparison

A C program that reads the listings.csv file, stores the data using C structures, sorts the listings by host_name and price, and writes the sorted data into two new CSV files.
## Requirements

- GCC compiler
- listings.csv

## Files
- listings_sorted_host_name.csv - Listings sorted by host name
- listings_sorted_price.csv - Listings sorted by price

## Compile

```bash
gcc lab6.c -o lab6
```

## Run

```bash
./lab6

```

## Example

```text
The program creates two new files:

listings_sorted_host_name.csv
listings_sorted_price.csv

The first file contains the listings sorted alphabetically by host_name.

The second file contains the listings sorted from lowest to highest price.
```

## How It Works
The program opens listings.csv using fopen() and reads each line using fgets().

The data is stored in a struct listing. The strtok() function is used to separate the comma-separated fields.

The C library qsort() function is then used to sort the structures. One comparison function sorts the listings by host_name, while another comparison function sorts them by price.

Finally, fprintf() is used to write the sorted structures into two new CSV files.