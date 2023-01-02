#include "matrix.h"
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <pthread.h>
#include <thread>
#include <fstream>
#include <string>

using namespace mat;
using namespace std;

struct arg_struct
{
   int arg1;
   int arg2;
};

typedef void * (*THREADFUNCPTR)(void *);

class Image{
    public:
        Matrix<int> data;

        string imageType;

        int totalPixels;
        int x_size;
        int y_size;

        Image()
		{
		}

        Image(string image_path)
		{
            openImage(image_path);
        }

        void openImage(string image_path)
		{
            string mytext;
            ifstream MyReadFile(image_path);

            getline(MyReadFile,mytext);
            getline(MyReadFile,mytext);

            cout << mytext << "\n";
            string s_y_size = mytext.substr(0,3);
            cout <<"x_size: "<< s_y_size;
            string s_x_size = mytext.substr(4,3);
            cout << "y_size: "<<s_x_size;
            this->x_size = atoi( s_x_size.c_str() );
            this->y_size = atoi( s_y_size.c_str() );
            cout << "image size: " << this->x_size << "\n";

            getline(MyReadFile, mytext);

            cout << mytext << "\n";
            this->data.set_size(this->x_size,this->y_size);
            cout<< "y_size: "<< y_size;

            int y = 0;
            int x = 0;

            while (getline(MyReadFile, mytext)) 
			{
                int begin_position = 0;
                int end_position = 0;

                for(int i = 0; i < mytext.length(); i++)
				{
                    end_position = i;
                    string substring;
                    int actual_pixel;

                    if(
                        (mytext.at(i) == ' ' || mytext.at(i) == '\n')
                        && y < this->y_size 
                        && x < this->x_size
                    )
					{
                        substring = (mytext.substr(begin_position, (end_position - begin_position)));
                        actual_pixel = atoi(substring.c_str());

                        this->data.at(x, y) = actual_pixel;

                        begin_position = end_position+1;

                        y++;
                    }
                }

                x++;
                y = 0;
            }

            MyReadFile.close();
        }

        void saveImage(string file_path)
		{
            ofstream myfile(file_path);
            myfile<< "P2"<< endl;
            myfile << to_string(this->y_size) <<" " << to_string(this->x_size) << endl;
            myfile<< "255"<< endl;

            if(myfile.is_open())
            {
                string str;
                int number = 0;

                for (int i = 0; i < this->x_size; i++)
				{
                    str = "";

                    for (int j = 0; j< this->y_size; j++)
					{
                        str.append(to_string(this->data.at(i, j)));
                        str.append(" ");
                    }

                    myfile<<str<< endl;
                }
                myfile.close();
            }
            else cerr<<"Unable to open file";
        }

        void saltAndPepper(int nThreads)
		{            
            int numCols = data.get_num_cols();
            int numRows = data.get_num_rows();
            int n_cores = (int)(thread::hardware_concurrency());

            int rc; 

            vector<arg_struct> args;
            args.reserve(n_cores);
            vector<thread> t1;

            int numberOfRows = (int)(numRows / n_cores);

            for(int i = 0; i < n_cores; i++)
			{
                args[i].arg1 = i * numberOfRows;

                if ((i * numberOfRows + numberOfRows) > numberOfRows)
				{
                    args[i].arg2 = numberOfRows * i - 1;
                }
                else
				{
                    args[i].arg2 = i + numberOfRows * n_cores;
                }

                some_threads_2.push_back(thread(&Image::saltAndPepperAsync,this, args[i]));
            }

            for (auto& t: some_threads_2) t.join();
        }

        vector<Matrix<int>> getSubImages(int NThreads)
		{
            int numCols = data.get_num_cols();
            int numRows = data.get_num_rows();
            int totalPixels = numCols * numRows;
            
            for (int i = 0; i < NThreads; i++){

            }
        }

        int median(Matrix<int> subMatrix)
		{
            int numCols = subMatrix.get_num_cols();
            int numRows = subMatrix.get_num_rows();
            int vectorSize = numCols * numRows;
            int numbersArray[numCols + numRows];
            int out_values = 0;
            int actualPosition = 0;

            vector<int> numberVector(numbersArray, numbersArray+vectorSize);

            for (int i = 0; i < numCols; i++)
			{
                for (int  j = 0; j < numRows; j++)
				{
                    if (subMatrix.at(i, j) == -1)
					{
                        out_values--;
                        continue;
                    }

                    numberVector[actualPosition] = subMatrix.at(i, j);
                    actualPosition++;
                }
            }

            sort(numberVector.begin(), numberVector.begin() + actualPosition);
            return numberVector[(int)actualPosition / 2];
        }

        Matrix<int> getSubMatrix(int x, int y, int matrixSize)
		{
            Matrix<int> newMatrix;
            newMatrix.set_size(matrixSize, matrixSize);

            int xStartPoint = x - (int)(matrixSize  / 2 );
            int yStartPoint = y - (int)(matrixSize / 2 );

            int r; 

            for(int i = 0; i < matrixSize; i++)
			{
                for (int j = 0; j < matrixSize; j++)
				{
                    if(
                        (xStartPoint + i < 0 || yStartPoint + j < 0) || 
                        (
                        i - xStartPoint >=this->data.get_num_rows() || 
                        j - yStartPoint >= this->data.get_num_cols()
                        )||
                        (
                        i + xStartPoint >= this->data.get_num_rows() || 
                        j + yStartPoint >= this->data.get_num_cols()
                        )
                    )
					{
                        newMatrix.at(i, j) = -1;
                    }
                    else
					{
                        newMatrix.at(i, j) = this->data.at(xStartPoint + i, yStartPoint + j);
                    }
                }
            }

            return newMatrix;
        }
    
        void pixelToMedian(int x, int y)
		{
            this->data.at(x, y) = median(this->getSubMatrix(x, y, 3));
        }

        private:

            void foo_func() 
			{ 
				std::cout << "Hello\n\n\n\n"; 
			}

            std::vector<std::thread> some_threads;

            void saltAndPepperAsync(arg_struct a)
			{
                cout << "thread created\n";

                int x = a.arg1;
                int xf = a.arg2;
                int numCols = this->data.get_num_cols();
                int numRows = this->data.get_num_rows();

                for (int i = x; i < xf; i++ )
				{
                    for ( int j = 0; j < numCols; j++)
					{
                        this->data.at(i, j) = this->median(getSubMatrix(i, j, 3));
                    }
                }
            }

            std::vector<std::thread> some_threads_2;
};

int main(int argc,char* argv[])
{
	string filename = argv[1];

	auto x = Matrix<int>(10, 10);
	auto y = Matrix<int>(10, 10);
	
	for(int i = 0; i < x.get_num_rows(); i++)
	{
		for(int j = 0; j < x.get_num_cols(); j++)
		{
            if(j < 2)
			{
                x.at(i, j) = -1;
                continue;
            }
			x.at(i, j) = 0 + i;
		}
	}
	
	Image image(filename);

    image.saltAndPepper(3);
    cout << "\n" << "\n";
    image.saveImage("./Images/dataset_hard/001.pgm"); //Trocar pelo nome da imagem
	
	return 0;
}