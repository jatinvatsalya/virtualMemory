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
        for ( i = 0; i < *frame_cnt; ++i )
        {
            frame_pool[ i ] = frame_pool[ i + 1 ];
        }
        --(*frame_cnt);
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

int count_page_faults_fifo(
    struct PTE page_table[TABLEMAX],
    int table_cnt,
    int refrence_string[REFERENCEMAX],
    int reference_cnt,
    int frame_pool[POOLMAX],
    int frame_cnt)
{
    int i;
    int iSmallestAT = 0;
    int frame;
    int initialized = 0;
    int iRefrenceString;
    int timestamp = 1;
    int result = 0;

    for ( iRefrenceString = 0; iRefrenceString < reference_cnt; ++iRefrenceString )
    {
        if ( page_table[refrence_string[iRefrenceString]].is_valid )
        {
            page_table[refrence_string[iRefrenceString]].last_access_timestamp = timestamp++;
            page_table[refrence_string[iRefrenceString]].reference_count += 1;
        }
        else if ( frame_cnt )
        {
            page_table[refrence_string[iRefrenceString]].frame_number = frame_pool[0];
            for ( i = 0; i < frame_cnt; ++i )
            {
                frame_pool[ i ] = frame_pool[ i + 1 ];
            }
            page_table[refrence_string[iRefrenceString]].last_access_timestamp = timestamp++;
            page_table[refrence_string[iRefrenceString]].reference_count += 1;
            ++result;
        }
        else
        {
            ++result;
            for ( i = 0; i < table_cnt; ++i )
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
            if ( initialized )
            {
                page_table[refrence_string[iRefrenceString]].is_valid = 1;
                page_table[refrence_string[iRefrenceString]].frame_number = page_table[iSmallestAT].frame_number;
                page_table[refrence_string[iRefrenceString]].arrival_timestamp = timestamp;
                page_table[refrence_string[iRefrenceString]].last_access_timestamp = ++timestamp;
                page_table[refrence_string[iRefrenceString]].reference_count = 1;

                page_table[iSmallestAT].is_valid = 0;
                page_table[iSmallestAT].frame_number = -1;
                page_table[iSmallestAT].arrival_timestamp = 0;
                page_table[iSmallestAT].last_access_timestamp = 0;
                page_table[iSmallestAT].reference_count = 0;
            }
        }
    }

    return result;
}

