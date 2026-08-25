import React from 'react';
import { BookOpen, CheckCircle2 } from 'lucide-react';

export const ComplexityTable: React.FC = () => {
  const dsaMatrix = [
    {
      name: 'Binary Max-Heap / Min-Heap',
      purpose: 'Order Book Buy (Max) & Sell (Min) Priority Queues',
      timeBest: 'O(1)',
      timeAvg: 'O(log N)',
      timeWorst: 'O(log N)',
      space: 'O(N)',
      file: 'backend/src/dsa/MaxHeap.hpp',
      defense: 'Guarantees the best bid price and lowest ask price are matched in logarithmic time per fill.',
    },
    {
      name: 'Price-Time Matching Engine',
      purpose: 'FIFO Order Book Execution & Price Crossings',
      timeBest: 'O(1)',
      timeAvg: 'O(K log N)',
      timeWorst: 'O(K log N)',
      space: 'O(N)',
      file: 'backend/src/trading/MatchingEngine.cpp',
      defense: 'Breaks price ties chronologically using timestamps embedded in heap comparators.',
    },
    {
      name: 'Adjacency List Graph',
      purpose: 'Stock Market Sectors & Supply-Chain Shockwaves',
      timeBest: 'O(V + E)',
      timeAvg: 'O(V + E)',
      timeWorst: 'O(V + E)',
      space: 'O(V + E)',
      file: 'backend/src/dsa/Graph.hpp',
      defense: 'Accurately captures contagion resistance and sector correlation topology.',
    },
    {
      name: "Dijkstra's Shortest Path",
      purpose: 'Lowest-Resistance Market Shock Propagation',
      timeBest: 'O((V + E) log V)',
      timeAvg: 'O((V + E) log V)',
      timeWorst: 'O((V + E) log V)',
      space: 'O(V)',
      file: 'backend/src/dsa/Graph.hpp',
      defense: 'Uses Min-Priority Queue to compute minimum shockwave resistance paths to all correlated stocks.',
    },
    {
      name: 'Separate Chaining Hash Table',
      purpose: 'Symbol -> Stock and Token -> Session Cache',
      timeBest: 'O(1)',
      timeAvg: 'O(1)',
      timeWorst: 'O(N)',
      space: 'O(N + M)',
      file: 'backend/src/dsa/HashMap.hpp',
      defense: 'Guarantees instant constant-time asset lookup with dynamic rehashing at load factor > 0.75.',
    },
    {
      name: 'Merge Sort',
      purpose: 'Leaderboard Ranking by Portfolio Valuation',
      timeBest: 'O(N log N)',
      timeAvg: 'O(N log N)',
      timeWorst: 'O(N log N)',
      space: 'O(N)',
      file: 'backend/src/dsa/Sorting.hpp',
      defense: 'Ensures stable, deterministic ranking of competitors without modifying equal P&L precedence.',
    },
    {
      name: 'Quick Sort',
      purpose: 'Top Market Gainers & Losers Ranking',
      timeBest: 'O(N log N)',
      timeAvg: 'O(N log N)',
      timeWorst: 'O(N²)',
      space: 'O(log N)',
      file: 'backend/src/dsa/Sorting.hpp',
      defense: 'High performance in-place partitioning for fast live ticker sorting.',
    },
    {
      name: 'FIFO Queue',
      purpose: 'Incoming Order Buffer & BFS Wave Propagation',
      timeBest: 'O(1)',
      timeAvg: 'O(1)',
      timeWorst: 'O(1)',
      space: 'O(N)',
      file: 'backend/src/dsa/Queue.hpp',
      defense: 'Strictly preserves first-in-first-out arrival sequence before matching engine ingestion.',
    },
    {
      name: 'LIFO Stack',
      purpose: 'User Action Undo & DFS Recursion Tracing',
      timeBest: 'O(1)',
      timeAvg: 'O(1)',
      timeWorst: 'O(1)',
      space: 'O(N)',
      file: 'backend/src/dsa/Stack.hpp',
      defense: 'Provides instantaneous last-in-first-out rollback for simulated order drafts.',
    },
  ];

  return (
    <div className="p-6 rounded-2xl glass-card border border-slate-800/80 shadow-2xl space-y-4">
      <div className="flex items-center gap-2 pb-3 border-b border-slate-800">
        <BookOpen className="w-5 h-5 text-emerald-400" />
        <h3 className="font-bold text-base text-slate-100">
          DSA Academic Reference & Viva Defense Matrix
        </h3>
      </div>

      <div className="overflow-x-auto">
        <table className="w-full text-left font-mono text-xs">
          <thead>
            <tr className="border-b border-slate-800 text-slate-400 uppercase text-[10px] tracking-wider">
              <th className="py-3 px-3">Data Structure / Algorithm</th>
              <th className="py-3 px-3">TradeVerse Feature</th>
              <th className="py-3 px-3">Time Complexity (Avg)</th>
              <th className="py-3 px-3">Space Complexity</th>
              <th className="py-3 px-3">C++ Implementation File</th>
            </tr>
          </thead>
          <tbody className="divide-y divide-slate-800/60">
            {dsaMatrix.map((item, idx) => (
              <tr key={idx} className="hover:bg-slate-800/30 transition-colors">
                <td className="py-3 px-3 font-bold text-slate-100">{item.name}</td>
                <td className="py-3 px-3 text-slate-300">{item.purpose}</td>
                <td className="py-3 px-3 font-bold text-emerald-400">{item.timeAvg}</td>
                <td className="py-3 px-3 text-blue-400">{item.space}</td>
                <td className="py-3 px-3 text-slate-400 text-[11px]">{item.file}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};