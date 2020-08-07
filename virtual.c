#include "oslabs.h"

int process_page_access_fifo(
    struct PTE page_table[TABLEMAX],
    int *table_cnt,
    int page_number,
    int frame_pool[POOLMAX],
    int *frame_cnt,
    int current_timestamp)
{
    int i;
    int iSmallestAT = 0;
    int frame;
    int initialized = 0;
    int result;

    if ( page_table[page_number].is_valid )
    {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        result = page_table[page_number].frame_number;
    }
    else if ( *frame_cnt )
    {
        page_table[page_number].frame_number = frame_pool[0];
        for ( i = 0; i < (POOLMAX - 1); ++i )
        {
            frame_pool[ i ] = frame_pool[ i + 1 ];
        }
        frame_pool[ POOLMAX - 1 ] = 0;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count += 1;
        result = page_table[page_number].frame_number;
    }
    else
    {
        for ( i = 0; i < *table_cnt; ++i )
        {
            if ( page_table[i].is_valid )
            {
                if ( ! initialized )
                {
                    initialized = 1;
                    frame = page_table[i].frame_number;
                    iSmallestAT = i;
                }
                else
                {
                    if ( frame > page_table[i].frame_number )
                    {
                        frame = page_table[i].frame_number;
                        iSmallestAT = i;
                    }
                }
            }
        }
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = page_table[iSmallestAT].frame_number;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;

        page_table[iSmallestAT].is_valid = 0;
        page_table[iSmallestAT].frame_number = -1;
        page_table[iSmallestAT].arrival_timestamp = 0;
        page_table[iSmallestAT].last_access_timestamp = 0;
        page_table[iSmallestAT].reference_count = 0;
        result = page_table[page_number].frame_number;
    }

    return result;
}
