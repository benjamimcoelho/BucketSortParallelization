
declare -a nodeList1
declare -a nodeList2
nodeList1=("compute-113-3" "compute-113-17" "compute-134-6")
nodeList2=("compute-165-1" "compute-881-2")

make

for node in "${nodeList1[@]}"; do
    echo  srun --partition=day --nodelist="$node" /home/pg47238/TP/papi.out -P -A
    srun --partition=day --nodelist="$node" /home/pg47238/TP/papi.out -P -A >> ./csvs/"$node".csv || (echo ERROR "$node" && echo ERROR >> ./csvs/"$node".csv)
done

for node in "${nodeList2[@]}"; do
    echo  srun --partition=day --nodelist="$node" /home/pg47238/TP/main.out -P -A
    srun --partition=day --nodelist="$node" /home/pg47238/TP/main.out -P -A >> ./csvs/"$node".csv || (echo ERROR "$node" && echo ERROR >> ./csvs/"$node".csv)
done
