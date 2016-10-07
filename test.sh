#!/bin/bash
for j in {1,2,4,8,16,24}
do
    time ./a.out test.png test_haha.png $j 
done
