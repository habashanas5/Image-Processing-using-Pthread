#include <iostream>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

#define KERNEL_SIZE 3

class ParallelFilter {
private:
    int rows;
    int cols;
    int** myImage;
    int** kernel;
    int numThreads;

public:
    ParallelFilter(int** image, int** myKernel, int myrows, int mycols, int threads)
        : myImage(image), kernel(myKernel), rows(myrows), cols(mycols), numThreads(threads) {}

    void dofunction() {
        int** resultImage = new int*[rows - KERNEL_SIZE + 1];
        for (int i = 0; i < rows - KERNEL_SIZE + 1; i++) {
            resultImage[i] = new int[cols - KERNEL_SIZE + 1];
        }

        FILE* FinalPthreadOutput = fopen("parallelfinalOutPut.txt", "w");
        if (FinalPthreadOutput == NULL) {
            cout << "Can't open output file ...." << endl;
            exit(EXIT_FAILURE);
        }

        clock_t start_time = clock(); // Record the start time of the function filterImage
        applyFilterParallel(resultImage);
        clock_t end_time = clock(); // Record the end time of the function filterImage
        double execution_time = ((double)(end_time - start_time) / (double)CLOCKS_PER_SEC) * 10;
        cout << "Execution time in seconds: " << execution_time << endl;

        // write result to the output file
        for (int i = 0; i < rows - KERNEL_SIZE + 1; i++) {
            for (int j = 0; j < cols - KERNEL_SIZE + 1; j++) {
                // Write to the text file
                fprintf(FinalPthreadOutput, "%d ", resultImage[i][j]);
            }
            fprintf(FinalPthreadOutput, "\n");
        }

        fclose(FinalPthreadOutput);
    }

    void applyFilterParallel(int** resultImage) {
    pthread_t* threads = new pthread_t[numThreads];
    int startRow, endRow;

    for (int i = 0; i < numThreads; i++) {
        startRow = i * (rows - KERNEL_SIZE + 1) / numThreads;
        endRow = (i + 1) * (rows - KERNEL_SIZE + 1) / numThreads;

        pthread_create(&threads[i], NULL, threadFunction, new ThreadData(this, resultImage, startRow, endRow));
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    delete[] threads;
    
	}

    static void* threadFunction(void* data) {
        ThreadData* threadData = static_cast<ThreadData*>(data);
        threadData->filter->applyFilter(threadData->resultImage, threadData->startRow, threadData->endRow);
        delete threadData;

        return nullptr;
    }

    void applyFilter(int** resultImage, int startRow, int endRow) const {
        int sum = 0;

        for (int i = startRow; i < endRow; i++) {
            for (int j = 0; j < (cols - KERNEL_SIZE + 1); j++) {
                sum = 0;
                for (int m = 0; m < KERNEL_SIZE; m++) {
                    for (int n = 0; n < KERNEL_SIZE; n++) {
                        sum += myImage[i + m][j + n] * kernel[m][n];
                    }
                }
                resultImage[i][j] = sum;
            }
        }
    }

private:
    struct ThreadData {
        ParallelFilter* filter;
        int** resultImage;
        int startRow;
        int endRow;

        ThreadData(ParallelFilter* f, int** r, int start, int end) : filter(f), resultImage(r), startRow(start), endRow(end) {}
    };
};

int main(int argc, char *argv[])
{
    // Check if there are 3 arguments for input image file, kernel file, and the output file
    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <input_image_file> <kernel_file> <output_image_file> <number_of_thread>" << endl;
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

    FILE* PthreadoutputFile = fopen(argv[3], "w");
    if (PthreadoutputFile == NULL) {
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
            fprintf(PthreadoutputFile, "%d ", value);
        }
        fprintf(PthreadoutputFile, "\n");
    }  
    
    int numberOfRow = matrix.size();
    int numberOfColumn = maxColumns;
    int maxThreads = atoi(argv[4]); 
    int sizeOfArray = numberOfRow * numberOfColumn;
    int sizeOfArrayInBytes = sizeOfArray * sizeof(int);
    fclose(PthreadoutputFile);
    
    //cout << "Size of an array Image : " <<  sizeOfArray <<  endl;
    //cout << "Size of an array Image in bytes : " <<  sizeOfArrayInBytes <<  endl;
	//cout << "Number of row of an array Image : " <<  numberOfRow <<  endl;  
	//cout << "Number of column of an array Image : " <<  numberOfColumn <<  endl;
	cout << "Number of thread: " << maxThreads << endl;
    cout << endl; 
    
    FILE* myOutPut = fopen("oimage.txt", "r");
    if (myOutPut == NULL) {
        cout << "Can't open output file ...." << endl;
        return EXIT_FAILURE;
    }
    
    int** kernal = new int*[KERNEL_SIZE];
    for (int i = 0; i < KERNEL_SIZE; ++i) {
        kernal[i] = new int[KERNEL_SIZE];
    }
    
    cout << "Kernel " << endl;
    for (int i = 0; i < KERNEL_SIZE; i++) {
        for (int j = 0; j < KERNEL_SIZE; j++) {
             fscanf(kernelFile, "%d", &kernal[i][j]);
             cout << kernal[i][j] << " ";
           }
           cout << endl;
       }
    cout << endl;
    
    numberOfRow = 253;
    numberOfColumn = 320;
    
    int** myImage = new int*[numberOfRow];
    for (int i = 0; i < 253; ++i) {
        myImage[i] = new int[numberOfColumn];    
    }
    
   // cout << "My Image:" << endl;
    for (int i = 0; i < numberOfRow; i++) {
        for (int j = 0; j < numberOfColumn; j++) {
             fscanf(myOutPut, "%d", &myImage[i][j]);
             //cout << myImage[i][j] << " ";
           }
           //cout << endl;
       }
    cout << endl;
    
    
    for (int numThreads = 1; numThreads <= maxThreads; numThreads++) {
        ParallelFilter a(myImage, kernal, numberOfRow, numberOfColumn, numThreads);
        a.dofunction();
    }
      return 0;
}
