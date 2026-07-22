#!/usr/bin/env python3
"""
SimpleSSD读时间分析脚本
计算每个workload的平均读时间，包括WL粒度和page粒度的分别统计
"""

import os
import pandas as pd
import numpy as np
from pathlib import Path

def analyze_read_time():
    """分析读时间数据"""
    
    # # 创建baseline结果目录
    # result_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/result_baseline")
    # result_dir.mkdir(exist_ok=True)
    
    # # 数据目录
    # data_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/data_baseline")


    # 创建结果目录
    result_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/result")
    result_dir.mkdir(exist_ok=True)
    
    # 数据目录
    data_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/data")

    
    # 存储所有workload的结果
    all_results = []
    
    # 查找所有读时间文件
    read_files = sorted(data_dir.glob("read_time*.csv"))
    
    if not read_files:
        print("未找到读时间文件！")
        return
    
    print(f"找到 {len(read_files)} 个读时间文件")
    
    for read_file in read_files:
        print(f"\n处理文件: {read_file.name}")
        
        try:
            # 读取CSV文件
            df = pd.read_csv(read_file)
            
            # 提取workload编号
            workload_num = read_file.stem.split('_')[-1]  # 从read_time1.csv中提取1
            
            # 基本统计信息
            total_requests = len(df)
            total_read_time = df['readTime'].sum()
            avg_read_time = df['readTime'].mean()
            median_read_time = df['readTime'].median()
            std_read_time = df['readTime'].std()
            min_read_time = df['readTime'].min()
            max_read_time = df['readTime'].max()
            
            # 按WL粒度分组统计
            page_granularity = df[df['isWLGranularity'] == 0]
            wl_granularity = df[df['isWLGranularity'] == 1]
            
            # Page粒度统计
            page_requests = len(page_granularity)
            page_avg_time = page_granularity['readTime'].mean() if page_requests > 0 else 0
            page_median_time = page_granularity['readTime'].median() if page_requests > 0 else 0
            
            # WL粒度统计
            wl_requests = len(wl_granularity)
            wl_avg_time = wl_granularity['readTime'].mean() if wl_requests > 0 else 0
            wl_median_time = wl_granularity['readTime'].median() if wl_requests > 0 else 0
            
            # 按WL粒度标识分组统计（简化版）
            short_requests = df[df['isWLGranularity'] == 0]  # 短请求（Page粒度）
            long_requests = df[df['isWLGranularity'] == 1]   # 长请求（WL粒度）
            
            short_count = len(short_requests)
            long_count = len(long_requests)
            
            short_avg_time = short_requests['readTime'].mean() if short_count > 0 else 0
            long_avg_time = long_requests['readTime'].mean() if long_count > 0 else 0
            
            # 存储结果
            result = {
                'workload': f"workload_{workload_num}",
                'total_requests': total_requests,
                'total_read_time': total_read_time,
                'avg_read_time': avg_read_time,
                'median_read_time': median_read_time,
                'std_read_time': std_read_time,
                'min_read_time': min_read_time,
                'max_read_time': max_read_time,
                'page_requests': page_requests,
                'page_avg_time': page_avg_time,
                'page_median_time': page_median_time,
                'wl_requests': wl_requests,
                'wl_avg_time': wl_avg_time,
                'wl_median_time': wl_median_time,
                'short_requests': short_count,
                'short_avg_time': short_avg_time,
                'long_requests': long_count,
                'long_avg_time': long_avg_time
            }
            
            all_results.append(result)
            
            # 打印详细统计信息
            print(f"  Workload {workload_num} 统计:")
            print(f"    总请求数: {total_requests}")
            print(f"    平均读时间: {avg_read_time:.2f} ps")
            print(f"    中位数读时间: {median_read_time:.2f} ps")
            print(f"    标准差: {std_read_time:.2f} ps")
            print(f"    最小/最大读时间: {min_read_time:.2f} / {max_read_time:.2f} ps")
            print(f"    Page粒度请求: {page_requests} 个, 平均时间: {page_avg_time:.2f} ps")
            print(f"    WL粒度请求: {wl_requests} 个, 平均时间: {wl_avg_time:.2f} ps")
            print(f"    短请求(Page粒度): {short_count} 个, 平均时间: {short_avg_time:.2f} ps")
            print(f"    长请求(WL粒度): {long_count} 个, 平均时间: {long_avg_time:.2f} ps")
            
        except Exception as e:
            print(f"处理文件 {read_file.name} 时出错: {e}")
            continue
    
    # 创建汇总DataFrame
    if all_results:
        summary_df = pd.DataFrame(all_results)
        
        # 保存详细结果到CSV
        summary_file = result_dir / "read_time_summary.csv"
        summary_df.to_csv(summary_file, index=False)
        print(f"\n详细结果已保存到: {summary_file}")
        
        # 创建简化版本（只包含关键指标）
        simple_summary = summary_df[['workload', 'total_requests', 'avg_read_time', 
                                   'page_avg_time', 'wl_avg_time', 'short_avg_time', 'long_avg_time',
                                   'short_requests', 'long_requests']].copy()
        simple_summary.columns = ['Workload', 'Total_Requests', 'Avg_Read_Time(ps)', 
                                'Page_Avg_Time(ps)', 'WL_Avg_Time(ps)', 
                                'Short_Avg_Time(ps)', 'Long_Avg_Time(ps)',
                                'Short_Count', 'Long_Count']
        
        simple_file = result_dir / "read_time_simple.csv"
        simple_summary.to_csv(simple_file, index=False)
        print(f"简化结果已保存到: {simple_file}")
        
        # 打印汇总表格
        print("\n" + "="*80)
        print("读时间分析汇总")
        print("="*80)
        print(simple_summary.to_string(index=False, float_format='%.2f'))
        
        # 计算总体统计
        print("\n" + "="*80)
        print("总体统计")
        print("="*80)
        total_all_requests = summary_df['total_requests'].sum()
        weighted_avg_time = (summary_df['total_read_time'].sum() / total_all_requests) if total_all_requests > 0 else 0
        
        print(f"所有workload总请求数: {total_all_requests}")
        print(f"加权平均读时间: {weighted_avg_time:.2f} ps")
        print(f"各workload平均读时间范围: {summary_df['avg_read_time'].min():.2f} - {summary_df['avg_read_time'].max():.2f} ps")
        
        # WL粒度效果分析
        wl_workloads = summary_df[summary_df['wl_requests'] > 0]
        if len(wl_workloads) > 0:
            print(f"\nWL粒度效果分析:")
            print(f"使用WL粒度的workload数: {len(wl_workloads)}")
            for _, row in wl_workloads.iterrows():
                if row['page_requests'] > 0 and row['wl_requests'] > 0:
                    speedup = row['page_avg_time'] / row['wl_avg_time'] if row['wl_avg_time'] > 0 else 0
                    print(f"  {row['workload']}: Page粒度 {row['page_avg_time']:.2f}ps vs WL粒度 {row['wl_avg_time']:.2f}ps (加速比: {speedup:.2f}x)")

def main():
    """主函数"""
    print("SimpleSSD读时间分析工具")
    print("="*50)
    
    analyze_read_time()
    
    print("\n分析完成！")

if __name__ == "__main__":
    main()
