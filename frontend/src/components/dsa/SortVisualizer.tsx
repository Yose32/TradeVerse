import React, { useState } from 'react';
import { BarChart3, Play, RefreshCw, SlidersHorizontal, ArrowUpDown } from 'lucide-react';
import { api } from '../../services/api';
import { SortTraceResult, SortStepLog } from '../../types';

export const SortVisualizer: React.FC = () => {
  const [algorithm, setAlgorithm] = useState<'MERGE_SORT' | 'QUICK_SORT'>('MERGE_SORT');
  const [steps, setSteps] = useState<SortStepLog[]>([]);
  const [currentStepIdx, setCurrentStepIdx] = useState<number>(0);
  const [isPlaying, setIsPlaying] = useState<boolean>(false);

  const fetchTrace = async (algo: 'MERGE_SORT' | 'QUICK_SORT') => {
    try {
      const res = await api.runSortTrace(algo);
      setSteps(res.steps);
      setCurrentStepIdx(0);
    } catch (e) {
      console.error(e);
    }
  };

  React.useEffect(() => {
    fetchTrace(algorithm);
  }, [algorithm]);

  const handlePlay = () => {
    if (steps.length === 0) return;
    setIsPlaying(true);
    let step = 0;
    const interval = setInterval(() => {
      if (step < steps.length - 1) {
        step++;
        setCurrentStepIdx(step);
      } else {
        clearInterval(interval);
        setIsPlaying(false);
      }
    }, 600);
  };

  const currentStep = steps[currentStepIdx] || null;
  const currentArray = currentStep ? currentStep.array : [65, 28, 14, 88, 32, 95, 45, 12, 77, 50];

  return (
    <div className="p-6 rounded-2xl glass-card border border-slate-800/80 shadow-2xl space-y-6">
      <div className="flex flex-wrap items-center justify-between gap-4 pb-4 border-b border-slate-800">
        <div>
          <div className="flex items-center gap-2">
            <ArrowUpDown className="w-5 h-5 text-blue-400" />
            <h3 className="font-bold text-base text-slate-100">Sorting Algorithms Visualizer</h3>
            <span className="text-xs font-mono px-2 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
              Leaderboard & Ranking Core
            </span>
          </div>
          <p className="text-xs text-slate-400 mt-1">
            Compare stable O(N log N) Merge Sort (used for Leaderboards) with in-place Quick Sort (used for Top Gainers/Losers).
          </p>
        </div>

        <div className="flex items-center gap-1.5 p-1 rounded-xl bg-slate-900 border border-slate-800">
          <button
            onClick={() => setAlgorithm('MERGE_SORT')}
            className={`px-3 py-1.5 rounded-lg text-xs font-mono font-bold transition-all ${
              algorithm === 'MERGE_SORT'
                ? 'bg-blue-600 text-white shadow-md'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Merge Sort (Stable O(N log N))
          </button>
          <button
            onClick={() => setAlgorithm('QUICK_SORT')}
            className={`px-3 py-1.5 rounded-lg text-xs font-mono font-bold transition-all ${
              algorithm === 'QUICK_SORT'
                ? 'bg-purple-600 text-white shadow-md'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Quick Sort (In-Place Partition)
          </button>
        </div>
      </div>

      {/* Control Buttons */}
      <div className="flex flex-wrap items-center gap-3 bg-slate-900/60 p-3 rounded-xl border border-slate-800">
        <button
          onClick={handlePlay}
          disabled={isPlaying}
          className="flex items-center gap-2 px-4 py-1.5 rounded-lg bg-blue-600 hover:bg-blue-500 text-white text-xs font-bold shadow-sm transition-all disabled:opacity-50"
        >
          <Play className="w-3.5 h-3.5" />
          <span>Play Animation</span>
        </button>

        <button
          onClick={() => setCurrentStepIdx(0)}
          className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-xs font-semibold border border-slate-700 transition-colors"
        >
          <RefreshCw className="w-3.5 h-3.5" />
          <span>Restart</span>
        </button>

        <div className="flex items-center gap-2 ml-auto text-xs font-mono text-slate-400">
          <span>Step {currentStepIdx + 1} of {steps.length}</span>
        </div>
      </div>

      {/* Step Description */}
      {currentStep && (
        <div className="p-3 rounded-xl bg-black/40 border border-slate-800 font-mono text-xs text-slate-300 flex items-center gap-2">
          <span className="text-blue-400 font-bold">[C++ Step]:</span>
          <span>{currentStep.description}</span>
        </div>
      )}

      {/* Animated Bars */}
      <div className="h-56 p-6 rounded-xl bg-[#060a14] border border-slate-900 flex items-end justify-center gap-3">
        {currentArray.map((val, idx) => {
          const isHighlighted = currentStep?.highlighted.includes(idx);
          const heightPercent = (val / 100) * 100;

          return (
            <div key={idx} className="flex-1 flex flex-col items-center gap-2 max-w-12">
              <span className="text-[11px] font-mono font-bold text-slate-300">{val}</span>
              <div
                className={`w-full rounded-t-lg transition-all duration-300 ${
                  isHighlighted
                    ? 'bg-blue-500 shadow-lg shadow-blue-500/40 scale-105'
                    : 'bg-slate-700/80'
                }`}
                style={{ height: `${heightPercent}%` }}
              />
              <span className="text-[9px] font-mono text-slate-500">[{idx}]</span>
            </div>
          );
        })}
      </div>
    </div>
  );
};