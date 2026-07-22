#!/bin/bash
folder_name='trace_msr'
pathold='test1.revised'
indexold='FTLINDEX = 1'

pe_list=(3000 4000 5000 6000)
# time_list=("1m" "3m" "6m" "12m")
chip_list=("3DV7" "X3_9070")
# model_list=("_not_group" "")
model_list=("_drrm")

# pe_list=(3000)
time_list=("1m")
# chip_list=("X3_9070")
# model_list=("_not_group")
trace_list=(28 11 4)

cacheold='CacheSize = 536870912'
blockpageold='Page = 512'

# for j in {1..8}; do
for j in "${trace_list[@]}"; do
    for chip in "${chip_list[@]}"; do
        for model in "${model_list[@]}"; do
            for pe in "${pe_list[@]}"; do
                for time in "${time_list[@]}"; do
                    # 更新配置文件路径
                    path="${folder_name}/test${j}.revised"
                    sed -i "/^\[trace\]/,/^\[/ s|^File = .*|File = ${path}|g" config/sample.cfg
                    pathold=$path

                    # 根据芯片类型设置Page参数
                    if [ "$chip" = "3DV7" ]; then
                        page_value="4224"
                    elif [ "$chip" = "X3_9070" ]; then
                        page_value="4176"
                    else
                        page_value="512"  # 默认值
                    fi
                    
                    # 更新Page参数
                    sed -i "s/Page = [0-9]*/Page = ${page_value}/g" simplessd/config/sample.cfg

                    # 更新 FTLINDEX
                    sed -i "s/FTLINDEX = .*/FTLINDEX = ${j}/g" simplessd/config/sample.cfg
                    
                    echo '********************************************'
                    echo "Chip: $chip"
                    echo "Model: $model"
                    echo "PE: $pe"
                    echo "Time: $time"
                    echo "Path: $path"
                    echo "FTLINDEX = $j"
                    echo "Page = $page_value"
                    echo '********************************************'
                    
                    # 运行仿真，传入所有参数
                    ./simplessd-standalone config/sample.cfg simplessd/config/sample.cfg result "$chip" "$model" "$pe" "$time"
                    
                    echo '********************************************'
                    echo "Completed: Chip=$chip, Model=$model, PE=$pe, Time=$time"
                    echo '********************************************'
                done
            done
        done
    done
done

# str='s/'${cacheold}'/CacheSize=536870912/g'
# sed  -i $str simplessd/config/sample.cfg
sed -i "/^\[trace\]/,/^\[/ s|^File = .*|File = test1.revised|g" config/sample.cfg

sed -i 's/FTLINDEX = .*/FTLINDEX = 1/g' simplessd/config/sample.cfg

sed -i "s/Page = [0-9]*/${blockpageold}/g" simplessd/config/sample.cfg