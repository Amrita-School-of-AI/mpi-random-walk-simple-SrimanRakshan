#include <iostream>
#include <cstdlib> // For atoi, rand, srand
#include <ctime>   // For time
#include <mpi.h>

void run_walker();
void run_manager();

int boundary_limit;
int step_limit;
int proc_rank;
int proc_count;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    if (argc != 3)
    {
        if (proc_rank == 0)
        {
            std::cerr << "Usage: mpirun -np <p> " << argv[0] << " <boundary_limit> <step_limit>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    boundary_limit = atoi(argv[1]);
    step_limit = atoi(argv[2]);

    if (proc_rank == 0)
    {
        run_manager();
    }
    else
    {
        run_walker();
    }

    MPI_Finalize();
    return 0;
}

void run_walker()
{
    srand(time(NULL) + proc_rank);
    int current_pos = 0;

    for (int step_num = 1; step_num <= step_limit; ++step_num)
    {
        int move_dir = (rand() % 2 == 0) ? -1 : 1;
        current_pos += move_dir;

        if (current_pos < -boundary_limit || current_pos > boundary_limit)
        {
            std::cout << "Process " << proc_rank << ": Walker finished in " << step_num << " steps." << std::endl;
            int done_flag = 1;
            MPI_Send(&done_flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            break;
        }

        if (step_num == step_limit)
        {
            std::cout << "Process " << proc_rank << ": Walker finished in " << step_num << " steps." << std::endl;
            int done_flag = 1;
            MPI_Send(&done_flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
}

void run_manager()
{
    int total_walkers = proc_count - 1;
    int walkers_done = 0;

    while (walkers_done < total_walkers)
    {
        int recv_flag;
        MPI_Recv(&recv_flag, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        ++walkers_done;
    }

    std::cout << "Manager: All " << total_walkers << " walkers have completed their journey." << std::endl;
}
