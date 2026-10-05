#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINESIZE 1024
#define MAX_LISTINGS 22555

struct listing {
    int id, host_id, minimum_nights, number_of_reviews;
    int calculated_host_listings_count, availability_365;

    char *host_name;
    char *neighbourhood_group;
    char *neighbourhood;
    char *room_type;

    float latitude, longitude, price;
};


/* Parse one line from the CSV file */
struct listing getfields(char *line) {
    struct listing item;

    item.id = atoi(strtok(line, ","));
    item.host_id = atoi(strtok(NULL, ","));
    item.host_name = strdup(strtok(NULL, ","));
    item.neighbourhood_group = strdup(strtok(NULL, ","));
    item.neighbourhood = strdup(strtok(NULL, ","));
    item.latitude = atof(strtok(NULL, ","));
    item.longitude = atof(strtok(NULL, ","));
    item.room_type = strdup(strtok(NULL, ","));
    item.price = atof(strtok(NULL, ","));
    item.minimum_nights = atoi(strtok(NULL, ","));
    item.number_of_reviews = atoi(strtok(NULL, ","));
    item.calculated_host_listings_count = atoi(strtok(NULL, ","));
    item.availability_365 = atoi(strtok(NULL, ","));

    return item;
}


/* Compare two listings by host_name */
int compareHostName(const void *a, const void *b) {
    struct listing *listingA = (struct listing *)a;
    struct listing *listingB = (struct listing *)b;

    return strcmp(listingA->host_name, listingB->host_name);
}


/* Compare two listings by price */
int comparePrice(const void *a, const void *b) {
    struct listing *listingA = (struct listing *)a;
    struct listing *listingB = (struct listing *)b;

    if (listingA->price < listingB->price)
        return -1;

    if (listingA->price > listingB->price)
        return 1;

    return 0;
}


/* Write one listing to a CSV file */
void writeListing(FILE *fptr, struct listing item) {
    fprintf(fptr,
            "%d,%d,%s,%s,%s,%f,%f,%s,%f,%d,%d,%d,%d\n",
            item.id,
            item.host_id,
            item.host_name,
            item.neighbourhood_group,
            item.neighbourhood,
            item.latitude,
            item.longitude,
            item.room_type,
            item.price,
            item.minimum_nights,
            item.number_of_reviews,
            item.calculated_host_listings_count,
            item.availability_365);
}


/* Free dynamically allocated strings */
void freeListing(struct listing item) {
    free(item.host_name);
    free(item.neighbourhood_group);
    free(item.neighbourhood);
    free(item.room_type);
}


int main(int argc, char *args[]) {

    struct listing list_items[MAX_LISTINGS];

    char line[LINESIZE];

    int count = 0;
    int i;

    /* Open input CSV file */
    FILE *fptr = fopen("listings.csv", "r");

    if (fptr == NULL) {
        printf("Error reading input file listings.csv\n");
        exit(-1);
    }


    /*
     * Read the header line.
     * The first line contains the column names,
     * so we do not want to parse it as a listing.
     */
    fgets(line, LINESIZE, fptr);


    /* Read all listings */
    while (fgets(line, LINESIZE, fptr) != NULL) {

        if (count < MAX_LISTINGS) {
            list_items[count++] = getfields(line);
        }
    }


    fclose(fptr);


    printf("Number of listings read: %d\n", count);


    /*
     * Sort by host_name
     */
    qsort(
        list_items,
        count,
        sizeof(struct listing),
        compareHostName
    );


    /* Open file for host_name sorted data */
    FILE *hostFile = fopen("listings_sorted_host_name.csv", "w");

    if (hostFile == NULL) {
        printf("Error creating host name output file\n");
        exit(-1);
    }


    /* Write CSV header */
    fprintf(hostFile,
            "id,host_id,host_name,neighbourhood_group,neighbourhood,latitude,longitude,room_type,price,minimum_nights,number_of_reviews,calculated_host_listings_count,availability_365\n");


    /* Write sorted listings */
    for (i = 0; i < count; i++) {
        writeListing(hostFile, list_items[i]);
    }


    fclose(hostFile);


    /*
     * Sort by price
     */
    qsort(
        list_items,
        count,
        sizeof(struct listing),
        comparePrice
    );


    /* Open file for price sorted data */
    FILE *priceFile = fopen("listings_sorted_price.csv", "w");

    if (priceFile == NULL) {
        printf("Error creating price output file\n");
        exit(-1);
    }


    /* Write CSV header */
    fprintf(priceFile,
            "id,host_id,host_name,neighbourhood_group,neighbourhood,latitude,longitude,room_type,price,minimum_nights,number_of_reviews,calculated_host_listings_count,availability_365\n");


    /* Write sorted listings */
    for (i = 0; i < count; i++) {
        writeListing(priceFile, list_items[i]);
    }


    fclose(priceFile);


    /*
     * Free dynamically allocated memory
     */
    for (i = 0; i < count; i++) {
        freeListing(list_items[i]);
    }


    printf("Sorted files created successfully.\n");
    printf("1. listings_sorted_host_name.csv\n");
    printf("2. listings_sorted_price.csv\n");


    return 0;
}