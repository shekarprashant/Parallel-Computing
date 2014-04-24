// --*-c++-*--

#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "image.h"
#include "image_utils.h"

std::string parallel_main(int argc, char* argv[])
{
  if (argc < 5) {
    std::cout << "usage: " << argv[0] << " "
	      << "<in image> <out image> <filter type> <window size>"
              << std::endl;
    std::cout << " filter type = {mean|median} " << std::endl;
    exit(-1);
  }

  int rank, namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &namelen);

  int numprocs = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  int sqrt_numprocs = (int)ceil(sqrt(numprocs - 1));
  
  std::string inFilename(argv[1]);
  std::string outFilename(argv[2]);
  std::string filterType(argv[3]);
  int windowSize = atoi(argv[4]);

  outFilename = "p_" + outFilename;

	Image input, output, gsInput;
	int bufferHeader[8];
	int row = 0, col = 0, size= 0, start_row, end_row, start_col, end_col, start_info = -1;
	int act_start_row, act_end_row, act_start_col, act_end_col;
	int send_count = 0;
	std::vector<uint32_t> bufferImgData;

	double start_time = 0, end_time = 0;

	start_time = MPI_Wtime();
	std::cout << "[" << std::setprecision(6) <<end_time * 1000 <<"ms:";
	std::cout << " " << processor_name << " (P" << rank << ")]: ";
	std::cout << "Processing with filter: "<< filterType << " and window size:"<< windowSize << std::endl;
	std::cout << std::endl;

	if (rank == 0) 
	{
		std::cout << "[" << std::setprecision(6) <<end_time * 1000 <<"ms:";
		std::cout << " " << processor_name << " (P" << rank << ")]: ";
		std::cout << "* doing parallel image filtering ** " << std::endl;

		std::cout << "[" << std::setprecision(6) <<end_time * 1000 <<"ms:";
		std::cout << " " << processor_name << " (P" << rank << ")]: ";
		std::cout << "* reading regular tiff: " << argv[1] << std::endl;

		input.load_tiff(std::string(argv[1]));
		gsInput = input;
		gsInput.make_greyscale();

		if (numprocs > 1)
		{
			// Assign work
			int row_size = gsInput.height();
			int col_size = gsInput.width();

			output.resize(col_size, row_size);

			int row_block = row_size < sqrt_numprocs? row_size : (int)ceil((double)row_size/sqrt_numprocs);
			int col_block = col_size < sqrt_numprocs? col_size : (int)ceil((double)col_size/sqrt_numprocs);

			int num_blocks = (int)(ceil((double)row_size/row_block) * ceil((double)col_size/col_block));


			start_info = num_blocks;
			int pid = 0;
			for (pid = 0; pid < numprocs - 1 && pid < num_blocks; pid++)
				MPI_Ssend(&start_info, 1, MPI_INT, pid + 1, 0, MPI_COMM_WORLD);

			start_info = 0;
			for (; pid < numprocs - 1; pid++)
				MPI_Ssend(&start_info, 1, MPI_INT, pid + 1, 0, MPI_COMM_WORLD);

			for (int procs = 0; procs < num_blocks; procs += numprocs - 1)
			{
				for (int i = 0; i < numprocs - 1 && i + procs < num_blocks; i++)
				{
					act_start_row = row;
					act_end_row = row + row_block;
					act_end_row = act_end_row < row_size ? act_end_row : row_size;
					start_row = act_start_row - windowSize/2;
					start_row = start_row < 0 ? 0 : start_row;
					end_row = act_end_row + windowSize - windowSize/2; 
					end_row = end_row < row_size ? end_row : row_size;

					act_start_col = col;
					act_end_col = col + col_block;
					act_end_col = act_end_col < col_size ? act_end_col : col_size;
					start_col = act_start_col - windowSize/2;
					start_col = start_col < 0 ? 0 : start_col;
					end_col = act_end_col + windowSize - windowSize/2; 
					end_col = end_col < col_size ? end_col : col_size;

					size = (end_row - start_row)*(end_col - start_col);

					bufferImgData.clear();
					gsInput.get_part(bufferImgData, start_row, end_row, start_col, end_col);

					bufferHeader[0] = act_start_row;
					bufferHeader[1] = act_end_row;
					bufferHeader[2] = start_row;
					bufferHeader[3] = end_row;

					bufferHeader[4] = act_start_col;
					bufferHeader[5] = act_end_col;
					bufferHeader[6] = start_col;
					bufferHeader[7] = end_col;


					MPI_Ssend(bufferHeader, 8, MPI_INT, i%(numprocs-1) + 1, 4*send_count + 1, MPI_COMM_WORLD);
					MPI_Ssend(&bufferImgData[0], size, MPI_INT, i%(numprocs-1) + 1, 4*send_count + 2, MPI_COMM_WORLD);

					col += col_block;
					if (col >= col_size)
					{
						col = 0;
						row += row_block;
						if (row >= row_size)
							break;
					}
				}

				for (int i = 0; i < numprocs - 1 && i + procs < num_blocks; i++)
				{
					MPI_Recv(bufferHeader, 8, MPI_INT, i%(numprocs-1) + 1, 4*send_count + 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					act_start_row = bufferHeader[0];
					act_end_row = bufferHeader[1];
					start_row = bufferHeader[2];
					end_row = bufferHeader[3];

					act_start_col = bufferHeader[4];
					act_end_col = bufferHeader[5];
					start_col = bufferHeader[6];
					end_col = bufferHeader[7];

					size = (act_end_row - act_start_row)*(act_end_col - act_start_col);

					bufferImgData.clear();
					bufferImgData.resize(size);
					MPI_Recv(&bufferImgData[0], size, MPI_INT, i%(numprocs-1) + 1, 4*send_count + 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


					output.set_part(bufferImgData, act_start_row, act_start_col, act_end_row, act_end_col);
				}
				send_count++;
			}
		}
	}

	



	if (rank == 0)
	{
		if (numprocs <= 1)
		{
			output = gsInput;
			if (filterType == "mean") 
				output.image_filter_mean(windowSize);
			else if (filterType == "median") 
				output.image_filter_median(windowSize);
		}
	}
	else 
	{
		MPI_Recv(&start_info, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (start_info != 0)
		{
			for (int i = rank - 1; i < start_info; i += numprocs - 1)
			{
				MPI_Recv(bufferHeader, 8, MPI_INT, 0, 4*send_count + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				act_start_row = bufferHeader[0];
				act_end_row = bufferHeader[1];
				start_row = bufferHeader[2];
				end_row = bufferHeader[3];

				act_start_col = bufferHeader[4];
				act_end_col = bufferHeader[5];
				start_col = bufferHeader[6];
				end_col = bufferHeader[7];

				end_time = MPI_Wtime();
				std::cout << "[" << std::setprecision(6) <<(end_time - start_time) *1000<<"ms:";
				std::cout << " " << processor_name << " (P" << rank << ")]: ";
				std::cout << "Received image segment bordered by: (" << start_row <<","<<start_col<<") and (" << end_row - 1 <<","<<end_col  - 1<<"). Size:  " << end_row - start_row << "x" << end_col - start_col << std::endl;

				size = (end_row - start_row)*(end_col - start_col);

				bufferImgData.clear();
				bufferImgData.resize(size);

				MPI_Recv(&bufferImgData[0], size, MPI_INT, 0, 4*send_count + 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

				Image bufferImg(end_col - start_col, end_row - start_row, bufferImgData);

				if (filterType == "mean") 
					bufferImg.image_filter_mean(windowSize, act_start_col - start_col, act_start_row - start_row, act_end_col - start_col, act_end_row - start_row);
				else if (filterType == "median") 
					bufferImg.image_filter_median(windowSize, act_start_col - start_col, act_start_row - start_row, act_end_col - start_col, act_end_row - start_row);

				bufferImgData.clear();
				bufferImg.get_part(bufferImgData, act_start_row - start_row, act_end_row - start_row, act_start_col - start_col, act_end_col  - start_col);

				size = (act_end_row - act_start_row)*(act_end_col - act_start_col);

				MPI_Ssend(bufferHeader, 8, MPI_INT, 0, 4*send_count + 3, MPI_COMM_WORLD);
				MPI_Ssend(&bufferImgData[0], size, MPI_INT, 0, 4*send_count + 4, MPI_COMM_WORLD);

				send_count++;

				end_time = MPI_Wtime();
				std::cout << "[" << std::setprecision(6) <<(end_time - start_time) *1000<<"ms:";
				std::cout << " " << processor_name << " (P" << rank << ")]: ";
				std::cout << "Sent image segment bordered by: (" << act_start_row <<","<<act_start_col<<") and (" << act_end_row - 1 <<","<<act_end_col - 1<<"). Size:  " << act_end_row - act_start_row << "x" << act_end_col - act_start_col << std::endl;
			}
		}

	}
		


	if (rank == 0) {
		end_time = MPI_Wtime();
		std::cout << "[" << std::setprecision(6) <<(end_time - start_time) *1000<<"ms:";
		std::cout << " " << processor_name << " (P" << rank << ")]: ";
		std::cout << "Saving image: " 					
              << outFilename 
              << std::endl;
    	output.save_tiff_grey_8bit(outFilename);
	}

	end_time = MPI_Wtime();
	std::cout << "[" << std::setprecision(6) <<(end_time - start_time) *1000<<"ms:";
	std::cout << " " << processor_name << " (P" << rank << ")]: ";
   std::cout << "-- done --" << std::endl;
	std::cout << std::endl;

  return outFilename;
}
