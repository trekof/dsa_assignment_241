/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "ann/xtensor_lib.h"
#include "ann/dataset.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader
{
public:
private:
    Dataset<DType, LType> *ptr_dataset;
    int batch_size;
    bool shuffle;
    bool drop_last;
    /*TODO: add more member variables to support the iteration*/
    int element_left;
    int current_index;
    int num_batches;

public:
    DataLoader(Dataset<DType, LType> *ptr_dataset,
               int batch_size,
               bool shuffle = true,
               bool drop_last = false)
    {
        /*TODO: Add your code to do the initialization */
        if (!ptr_dataset)
        {
            throw std::invalid_argument("ptr_dataset cannot be null")
        }

        if (batch_size <= 0)
        {
            throw std::invalid_argument("batch_size should > 0")
        }

        this->ptr_dataset = ptr_dataset;
        this->batch_size = batch_size;
        this->shuffle = shuffle;
        this->drop_last = drop_last;
        this->current_index = 0;
        this->num_batches = ptr_dataset->len() / batch_size;
        this->element_left = (ptr_dataset->len() % batch_size != 0 && !drop_last) ? ptr_dataset->len() % batch_size : 0;
    }
    virtual ~DataLoader() {}

    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// START: Section                                                     //
    /////////////////////////////////////////////////////////////////////////
    /*TODO: Add your code here to support iteration on batch*/
    class BatchIterator
    {
    private:
        DataLoader<DType, LType> *dataloader;
        int current_batch_index;

    public:
        BatchIterator(DataLoader<DType, LType> *dataloader) : dataloader(dataloader), current_batch_index(0) {}

        BatchIterator &operator++()
        {
            current_batch_index++;
            return *this;
        }

        bool operator!=(const BatchIterator &other) const
        {
            return current_batch_index != other.current_batch_index;
        }

        Batch<DType, LType> operator*()
        {
            if (current_batch_index >= dataloader->num_batches)
            {
                throw std::out_of_range("Batch index out of range");
            }
            return dataloader->get_batch(current_batch_index);
        }
    };

    BatchIterator begin()
    {
        return BatchIterator(this);
    }

    BatchIterator end()
    {
        return BatchIterator(this);
    }

    Batch<DType, LType> get_batch(int batch_index)
    {
        if (batch_index < 0 || batch_index >= num_batches)
        {
            throw std::out_of_range("Batch index out of range");
        }

        int start_index = batch_index * batch_size;
        int end_index = start_index + batch_size;

        if (drop_last && batch_index == num_batches - 1 && element_left != 0)
        {
            end_index = start_index + element_left;
        }
        return Batch<DType, LType>(ptr_dataset->get_data_slice(start_index, end_index), ptr_dataset->get_label_slice(start_index, end_index));
    }

    /////////////////////////////////////////////////////////////////////////
    // The section for supporting the iteration and for-each to DataLoader //
    /// END: Section                                                       //
    /////////////////////////////////////////////////////////////////////////
};

#endif /* DATALOADER_H */
