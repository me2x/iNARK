#!/bin/bash
#for i in `seq 1 10`;
#do
#  echo $i
#  ./build/time/src/iNARK -i spec.xml --source task_1 --target task_2 --type 1 --depth 2 | tee -a log_$1
#done    
COUNTER=0
while [  $COUNTER -lt $2 ]; do
      echo The counter is $COUNTER
      for j in `seq 1 $3`;
      do
	for k in `seq 1 $3`;
	do
	  if [ $j -ne $k ]; then
	      #echo "source task $j target task $k" | tee -a log_d1_$1
	      ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 1 | tee -a log_d1__$COUNTER 
             ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 1
             echo "($j to $k) $?" >> log_status_d1__$COUNTER
             ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 2 | tee -a log_d2__$COUNTER
             ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 2
             echo "($j to $k) $?" >> log_status_d2__$COUNTER 
             ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 3 | tee -a log_d3__$COUNTER
             ../build/time/src/iNARK -i $1 --source task_$j --target task_$k --type 1 --depth 3
             echo "($j to $k) $?" >> log_status_d3__$COUNTER
              
	  fi
        done
      done
      let COUNTER=COUNTER+1
    done
    
COUNTER=0
while [  $COUNTER -lt $2 ]; do
    awk 'NR % 2 == 1' log_d1__$COUNTER | awk '{print $6}' | tee -a graph_build_1__$COUNTER
    awk 'NR % 2 == 0' log_d1__$COUNTER | awk '{print $6}' | tee -a graph_explore_1__$COUNTER
    #echo "sum" |tee -a graph_build_1_sums
    awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_1__$COUNTER | tee -a graph_build_1_avgs
    #explore devo "provare tutte" quindi mi serve la somma. poi l'avg viene fatto sotto
    echo "sum" |tee -a graph_explore_1__$COUNTER
    awk '{ sum += $1 } END { if (NR > 0) print sum}' graph_explore_1__$COUNTER | tee -a graph_explore_1_avgs
    
    awk 'NR % 2 == 1' log_d2__$COUNTER | awk '{print $6}' | tee -a graph_build_2__$COUNTER
    awk 'NR % 2 == 0' log_d2__$COUNTER | awk '{print $6}' | tee -a graph_explore_2__$COUNTER
    echo "sum" |tee -a graph_build_2__$COUNTER
    awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_2__$COUNTER | tee -a graph_build_2_avgs
    
    echo "sum" |tee -a graph_explore_2__$COUNTER
    awk '{ sum += $1 } END { if (NR > 0) print sum}' graph_explore_2__$COUNTER | tee -a graph_explore_2_avgs
    
    awk 'NR % 2 == 1' log_d3__$COUNTER | awk '{print $6}' | tee -a graph_build_3__$COUNTER
    awk 'NR % 2 == 0' log_d3__$COUNTER | awk '{print $6}' | tee -a graph_explore_3__$COUNTER
    echo "sum" |tee -a graph_build_3__$COUNTER
    awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_3__$COUNTER | tee -a graph_build_3_avgs
    
    echo "sum" |tee -a graph_explore_3__$COUNTER
    awk '{ sum += $1 } END { if (NR > 0) print sum}' graph_explore_3__$COUNTER | tee -a graph_explore_3_avgs
    
    let COUNTER=COUNTER+1
done


awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_1_avgs | tee global_build_avg_1
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_explore_1_avgs | tee global_explore_avg_1
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_2_avgs | tee global_build_avg_2
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_explore_2_avgs | tee global_eexplore_avg_2
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_build_3_avgs | tee global_build_avg_3
awk '{ sum += $1 } END { if (NR > 0) print sum / NR }' graph_explore_3_avgs | tee global_explore_avg_3

              sync; echo 3 > /proc/sys/vm/drop_caches
