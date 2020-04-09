#include <gv/gvsoc_proxy.hpp>
#include <unistd.h>


int main()
{
    Gvsoc_proxy *proxy = new Gvsoc_proxy("/home/haugoug/src/pulp-sdk/tests/pmsis_tests/quick/hello/build/gapuino/plt_config.json");

    if (proxy->open())
        return -1;

    for (int i=0; i<5; i++)
    {
        proxy->run();

        sleep(2);

        printf("Deactivate\n");


        int64_t timestamp = proxy->pause();

        printf("Paused at %ld\n", timestamp);

        sleep(5);

        proxy->remove_event_regex(".*");

        proxy->run();

        sleep(5);

        printf("Activate\n");
        proxy->pause();

        proxy->add_event_regex(".*");
    }

    proxy->pause();
    proxy->close();




    return 0;
}