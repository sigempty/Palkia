#!/usr/bin/env bash
PALKIA_MEMORY_WATERMARK=2048000 build/app/stage1/test_swap --connect rdma0.danyang-06:12000 --comm rdma --buf_num 4096 --buf_size 8192
