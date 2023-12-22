#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

#define KERNEL_SIZE 3


int main(int argc, char *argv[]) {
    // Check if there are 3 arguments for input image file, kernel file, and the output file
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <input_image_file> <kernel_file> <output_image_file>" << endl;
        exit(EXIT_FAILURE);
    }
    
    FILE* inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        cout << "Can't open image file ...." << endl;
        exit(EXIT_FAILURE);
    }
    
    FILE* kernelFile = fopen(argv[2], "r");
    if (kernelFile == NULL) {
        cout << "Can't open kernel file ...." << endl;
        exit(EXIT_FAILURE);
    }

    FILE* outputFile = fopen(argv[3], "w");
    if (outputFile == NULL) {
        cout << "Can't open output file ...." << endl;
        exit(EXIT_FAILURE);
    }
    
    FILE* FinalOutput = fopen("finalOutPut.txt", "w");
    if (FinalOutput == NULL) {
        cout << "Can't open output file ...." << endl;
        exit(EXIT_FAILURE);
    }
    
    vector<vector<int>> matrix;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), inputFile)) {
    
        stringstream ss(buffer);   // Create a stringstream from the buffer
        vector<int> row;          // Vector to store the values of the current row

        int value;
        while (ss >> value) {
            row.push_back(value); // Add the value to the row vector
        }

        matrix.push_back(row);  // Add the value to the row vector
    }

    fclose(inputFile);

    // Find the row with the maximum number of columns
    auto maxRow = max_element(matrix.begin(), matrix.end(), [](const auto& a, const auto& b) {
        return a.size() < b.size();
    });

    // Determine the maximum number of columns
    size_t maxColumns = (maxRow != matrix.end()) ? maxRow->size() : 0;

    // Fill the other rows with placeholder values (1)
    for (auto& row : matrix) {
        while (row.size() < maxColumns) {
            row.push_back(1);
        }
    }

    // Display the modified matrix and write to the output file
    for (const auto& row : matrix) {
        for (int value : row) {
            fprintf(outputFile, "%d ", value);
        }
        fprintf(outputFile, "\n");
    }
    
    int numberOfRow = matrix.size();
    int numberOfColumn = maxColumns;
    int kernal[KERNEL_SIZE][KERNEL_SIZE];
    int myImage[numberOfRow][320];
    int resultImage[numberOfRow - KERNEL_SIZE + 1][320 - KERNEL_SIZE + 1];
    
    int sizeOfArray = numberOfRow * numberOfColumn;
    int sizeOfArrayInBytes = sizeOfArray * sizeof(int);
	int numberOfRows = sizeof(myImage) / sizeof(myImage[0]);
	int numberOfColumns = sizeof(myImage[0]) / sizeof(myImage[0][0]);

    //cout << "Size of an array Image : " <<  sizeOfArray <<  endl;
	//cout << "Size of an array Image in bytes : " <<  sizeOfArrayInBytes <<  endl;
	//cout << "Number of row of an array Image : " <<  numberOfRows <<  endl;  
	//cout << "Number of column of an array Image : " <<  320 <<  endl;
    //cout << endl; 
    fclose(outputFile);
    
    FILE* myOutPut = fopen("oimage.txt", "r");
    if (myOutPut == NULL) {
        cout << "Can't open output file ...." << endl;
        return EXIT_FAILURE;
    }
    
    // Read and print kernel
    cout << "Kernel:" << endl;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        for (int j = 0; j < KERNEL_SIZE; j++) {
             fscanf(kernelFile, "%d", &kernal[i][j]);
             cout << kernal[i][j] << " ";
           }
           cout << endl;
       }
    cout << endl;
    
    //Read my Image
    ////cout << "My Image:" << endl;
    for (int i = 0; i < 253; i++) {
        for (int j = 0; j < 320; j++) {
             fscanf(myOutPut, "%d", &myImage[i][j]);
            // cout << myImage[i][j] << " ";
           }
          // cout << endl;
       }
       
    clock_t start_time = clock(); // Record the start time of the function filterImage
    int sum;

    for (int i = 0; i < (253 - KERNEL_SIZE + 1); i++) {
        for (int j = 0; j < (320 - KERNEL_SIZE + 1); j++) {
            sum = 0;
            for (int m = 0; m < KERNEL_SIZE; m++) {
                for (int n = 0; n < KERNEL_SIZE; n++) {
                    sum += myImage[i + m][j + n] * kernal[m][n];
                }
            }
            resultImage[i][j] = sum;
        }
    }
	
	clock_t end_time = clock();  // Record the end time of the function filterImage
	
	//Time taken for execution time 
	double execution_time = ((double) (end_time - start_time) / (double) CLOCKS_PER_SEC) * 10;
	cout << "Execution time in seconds: " << execution_time << endl;  
	
	//Write result to the output file
	for (int i = 0; i < 253 - KERNEL_SIZE + 1; i++) {
	   for (int j = 0; j < 320- KERNEL_SIZE + 1; j++) {
		       // Write to the text file
	           fprintf(FinalOutput, "%d ", resultImage[i][j]);
	    }
		    fprintf(FinalOutput, "\n");
	}
		
    fclose(FinalOutput);
    

    return 0;
}

