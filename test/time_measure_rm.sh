 
#!/bin/bash
COUNTER=0
while [  $COUNTER -lt $2 ]; do
rm log_d1__$COUNTER
rm graph_build_1__$COUNTER
rm graph_explore_1__$COUNTER

rm log_d2__$COUNTER
rm graph_build_2__$COUNTER
rm graph_explore_2__$COUNTER

rm log_d3__$COUNTER
rm graph_build_3__$COUNTER
rm graph_explore_3__$COUNTER

rm log_status_d1__$COUNTER
rm log_status_d2__$COUNTER
rm log_status_d3__$COUNTER
    let COUNTER=COUNTER+1
done


rm graph_build_1_avgs 
rm graph_build_2_avgs 
rm graph_build_3_avgs 
rm graph_explore_1_avgs 
rm graph_explore_2_avgs  
rm graph_explore_3_avgs