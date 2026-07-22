#!/usr/bin/env python3
"""
SimpleSSD结果比较脚本
比较baseline (全部page粒度) 和 WL粒度读取的性能差异
"""

import pandas as pd
import numpy as np
from pathlib import Path

def compare_results():
    """比较baseline和WL粒度的结果"""
    
    # 结果目录
    baseline_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/result_baseline")
    wl_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/result")
    result_dir = Path("/home/simplessd/simplessd_tlc/simplessd-standalone/result")

    # 读取baseline结果
    baseline_file = baseline_dir / "read_time_simple.csv"
    wl_file = wl_dir / "read_time_simple.csv"
    
    if not baseline_file.exists():
        print(f"Baseline文件不存在: {baseline_file}")
        return
    
    if not wl_file.exists():
        print(f"WL粒度文件不存在: {wl_file}")
        return
    
    # 读取数据
    baseline_df = pd.read_csv(baseline_file)
    wl_df = pd.read_csv(wl_file)
    
    print("SimpleSSD性能比较分析")
    print("="*80)
    print(f"Baseline (全部Page粒度): {baseline_file}")
    print(f"WL粒度读取: {wl_file}")
    print("="*80)
    
    # 创建比较结果
    comparison_results = []
    
    for _, baseline_row in baseline_df.iterrows():
        workload = baseline_row['Workload']
        
        # 查找对应的WL粒度结果
        wl_row = wl_df[wl_df['Workload'] == workload]
        
        if len(wl_row) == 0:
            print(f"警告: 未找到workload {workload} 的WL粒度结果")
            continue
        
        wl_row = wl_row.iloc[0]
        
        # 计算性能差异
        baseline_avg = baseline_row['Avg_Read_Time(ps)']
        wl_avg = wl_row['Avg_Read_Time(ps)']
        
        # 计算加速比 (baseline时间 / WL时间)
        speedup = baseline_avg / wl_avg if wl_avg > 0 else 0
        
        # 计算时间差异百分比
        time_diff_pct = ((wl_avg - baseline_avg) / baseline_avg * 100) if baseline_avg > 0 else 0
        
        # 统计请求分布
        baseline_tiny = baseline_row['Tiny_Count']
        baseline_medium = baseline_row['Medium_Count'] 
        baseline_huge = baseline_row['Huge_Count']
        baseline_total = baseline_row['Total_Requests']
        
        wl_tiny = wl_row['Tiny_Count']
        wl_medium = wl_row['Medium_Count']
        wl_huge = wl_row['Huge_Count']
        wl_total = wl_row['Total_Requests']
        
        # 计算WL粒度使用率
        wl_usage_pct = (wl_row['WL_Avg_Time(ps)'] > 0) * 100  # 如果有WL粒度请求则为100%
        
        result = {
            'Workload': workload,
            'Baseline_Avg_Time(ps)': baseline_avg,
            'WL_Avg_Time(ps)': wl_avg,
            'Speedup': speedup,
            'Time_Diff_Pct(%)': time_diff_pct,
            'Baseline_Tiny_Count': baseline_tiny,
            'Baseline_Medium_Count': baseline_medium,
            'Baseline_Huge_Count': baseline_huge,
            'WL_Tiny_Count': wl_tiny,
            'WL_Medium_Count': wl_medium,
            'WL_Huge_Count': wl_huge,
            'WL_Usage_Pct(%)': wl_usage_pct,
            'Baseline_Total': baseline_total,
            'WL_Total': wl_total
        }
        
        comparison_results.append(result)
    
    # 创建比较DataFrame
    comparison_df = pd.DataFrame(comparison_results)
    
    # 保存比较结果
    comparison_file = result_dir / "performance_comparison.csv"
    comparison_df.to_csv(comparison_file, index=False)
    print(f"比较结果已保存到: {comparison_file}")
    
    # 打印比较结果
    print("\n性能比较结果:")
    print("-"*80)
    
    # 选择关键列显示
    display_cols = ['Workload', 'Baseline_Avg_Time(ps)', 'WL_Avg_Time(ps)', 
                   'Speedup', 'Time_Diff_Pct(%)', 'WL_Usage_Pct(%)']
    display_df = comparison_df[display_cols].copy()
    display_df.columns = ['Workload', 'Baseline_Avg(ps)', 'WL_Avg(ps)', 
                         'Speedup', 'Time_Diff(%)', 'WL_Usage(%)']
    
    print(display_df.to_string(index=False, float_format='%.2f'))
    
    # 总体统计
    print("\n" + "="*80)
    print("总体性能分析")
    print("="*80)
    
    if len(comparison_df) > 0:
        avg_speedup = comparison_df['Speedup'].mean()
        avg_time_diff = comparison_df['Time_Diff_Pct(%)'].mean()
        
        print(f"平均加速比: {avg_speedup:.2f}x")
        print(f"平均时间差异: {avg_time_diff:.2f}%")
        
        # 分析WL粒度效果
        wl_workloads = comparison_df[comparison_df['WL_Usage_Pct(%)'] > 0]
        if len(wl_workloads) > 0:
            print(f"\n使用WL粒度的workload数: {len(wl_workloads)}")
            for _, row in wl_workloads.iterrows():
                print(f"  {row['Workload']}: 加速比 {row['Speedup']:.2f}x, 时间差异 {row['Time_Diff_Pct(%)']:.2f}%")
        else:
            print("\n没有workload使用WL粒度读取")
        
        # 请求分布分析
        print(f"\n请求分布分析:")
        for _, row in comparison_df.iterrows():
            print(f"  {row['Workload']}:")
            print(f"    Baseline: 微请求 {row['Baseline_Tiny_Count']}, 中请求 {row['Baseline_Medium_Count']}, 巨请求 {row['Baseline_Huge_Count']}")
            print(f"    WL粒度: 微请求 {row['WL_Tiny_Count']}, 中请求 {row['WL_Medium_Count']}, 巨请求 {row['WL_Huge_Count']}")

def main():
    """主函数"""
    compare_results()

if __name__ == "__main__":
    main()

