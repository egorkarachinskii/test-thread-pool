#include <iostream>
#include "threadpool.h"

unsigned int DEF_TASK_TM = 100;

int doTask(unsigned taskNum)
{
    std::cout <<std::endl<< "start task: " << taskNum << std::endl;
    int timeOutMs = DEF_TASK_TM*(taskNum+1);
    std::this_thread::sleep_for(std::chrono::milliseconds(timeOutMs));
    std::cout << "end task: " << taskNum << std::endl;
    return timeOutMs;
}

int main(int argc, char *argv[])
{
    unsigned numThreads =3;//default value
    if(argc>1)
        numThreads = atoi(argv[1]); //tmp solution

    ThreadPool pool(numThreads);
    std::vector< std::future<int> > results;

    for (std::string line;
             std::cout
             << "(1): start" << std::endl
             << "(2): stop" << std::endl
             << "(3): add tasks" << std::endl
             << "(4): get results" << std::endl
             << "type a command > " && std::getline(std::cin, line); )
    {
        if (!line.empty()) {
            system("clear");
            std::cout<< "last input: "<< line << std::endl;
            unsigned cmd = atoi(line.c_str());
            switch (cmd) {
            case 1:
               pool.start();
                break;
            case 2:
               pool.stop();
                break;
            case 3:
                for(int i = 0; i <numThreads*3; ++i)
                {
                    results.emplace_back(pool.add([i] {return doTask(i);}));
                }
               break;
            case 4:
                for(auto && result: results)
                    std::cout << result.get() << ' ';
                std::cout << std::endl;
                break;
            default:
                std::cout << "Error: unknown command" << std::endl;
                break;
            }

        }
    }

    return 0;
}
