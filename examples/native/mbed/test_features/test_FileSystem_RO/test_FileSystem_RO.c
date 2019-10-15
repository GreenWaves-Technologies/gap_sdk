#include "gap_common.h"
#include "gap_fs.h"

/* Variables used. */
#define BUF_SIZE 1024
#define COUNT 5

static fs_file_t *file;
static char buff[ BUF_SIZE + 1 ];
static uint32_t count_done = 0;

static void check_file( void )
{
    int32_t size = 0;
    for( count_done = 0; count_done < COUNT; count_done++ )
    {
        size = fs_read( file, BUF_SIZE, buff );
        printf("%ld Read %ld bytes : \n%s\n", count_done, size, buff);
        if( size < BUF_SIZE )
            fs_seek( file, 0 );
    }
}

int main( )
{
    printf("Executing Filesystem test\n");

    char name[] = "hello.txt";
    fs_config_t conf;
    fs_config_default( &conf );

    fs_handle_t *fs = fs_mount( FS_HYPER, &conf );

    if( fs == NULL ) {
        printf("Error FS mounting !\n");
        free( fs );
        exit(-1);
    }
    printf("FS mounted.\n");

    file = fs_open( fs, name, 0 );
    if( file == NULL )
    {
        printf("File open failed !\n");
        exit(-1);
    }
    printf("File %s open success.\n", file->name);

    check_file();

    fs_close( file );
    printf("File %s closed.\n", file->name);

    fs_unmount( fs );

    free( fs );
    printf("FS unmounted.\n");

    return 0;
}
