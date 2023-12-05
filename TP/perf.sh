#!/bin/bash

MY=/home/pg47164/ComputacaoParalela

perf record  ./main.out $1 $2
perf report -n > perfreport
