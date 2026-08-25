import React, { useState } from 'react';
import { Cpu, Layers, Network, ArrowUpDown, BookOpen } from 'lucide-react';
import { HeapVisualizer } from '../components/dsa/HeapVisualizer';
import { GraphVisualizer } from '../components/dsa/GraphVisualizer';
import { SortVisualizer } from '../components/dsa/SortVisualizer';
import { ComplexityTable } from '../components/dsa/ComplexityTable';

export const DSALabPage: React.FC = () => {
  const [activeTab, setActiveTab] = useState<'HEAP' | 'GRAPH' | 'SORT' | 'COMPLEXITY'>('HEAP');

  const tabs = [
    { id: 'HEAP', label: 'Binary Heap (Order Book)', icon: Layers },
    { id: 'GRAPH', label: 'Market Graph (Dijkstra/BFS)', icon: Network },
    { id: 'SORT', label: 'Sorting Suite (Merge/Quick)', icon: ArrowUpDown },
    { id: 'COMPLEXITY', label: 'Viva Reference & Complexities', icon: BookOpen },
  ];

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      {/* Header */}
      <div className="flex flex-wrap items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2">
            <Cpu className="w-6 h-6 text-blue-400" />
            <h1 className="text-2xl font-extrabold text-slate-100">TradeVerse DSA Laboratory</h1>
            <span className="text-xs font-mono px-2.5 py-0.5 rounded-full bg-accent/20 text-purple-300 border border-accent/30 font-bold">
              Academic Core
            </span>
          </div>
          <p className="text-xs text-slate-400 mt-0.5">
            Step through, inspect, and demonstrate every C++ Data Structure and Algorithm powering the platform
          </p>
        </div>

        {/* Tab Selector */}
        <div className="flex flex-wrap gap-1.5 p-1 rounded-xl bg-slate-900 border border-slate-800 font-mono text-xs">
          {tabs.map((tab) => {
            const Icon = tab.icon;
            const isActive = activeTab === tab.id;
            return (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id as any)}
                className={`flex items-center gap-2 px-3.5 py-2 rounded-lg font-semibold transition-all ${
                  isActive
                    ? 'bg-blue-600 text-white shadow-md'
                    : 'text-slate-400 hover:text-slate-200'
                }`}
              >
                <Icon className="w-4 h-4" />
                <span>{tab.label}</span>
              </button>
            );
          })}
        </div>
      </div>

      {/* Render Active DSA Visualizer */}
      <div className="space-y-6">
        {activeTab === 'HEAP' && <HeapVisualizer />}
        {activeTab === 'GRAPH' && <GraphVisualizer />}
        {activeTab === 'SORT' && <SortVisualizer />}
        {activeTab === 'COMPLEXITY' && <ComplexityTable />}
      </div>
    </div>
  );
};