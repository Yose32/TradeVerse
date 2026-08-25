import React, { useState } from 'react';
import { Play, RotateCcw, Plus, Trash2, ArrowUp, ArrowDown, Cpu } from 'lucide-react';

export const HeapVisualizer: React.FC = () => {
  const [heapType, setHeapType] = useState<'MAX' | 'MIN'>('MAX');
  const [heap, setHeap] = useState<number[]>([150, 110, 130, 80, 95, 105, 120]);
  const [inputValue, setInputValue] = useState<number>(140);
  const [highlightIdx, setHighlightIdx] = useState<number | null>(null);
  const [actionLog, setActionLog] = useState<string>('Initialized with 7 sample price levels.');

  const parent = (i: number) => Math.floor((i - 1) / 2);
  const leftChild = (i: number) => 2 * i + 1;
  const rightChild = (i: number) => 2 * i + 2;

  const compare = (a: number, b: number) => {
    return heapType === 'MAX' ? a > b : a < b;
  };

  const handleInsert = async () => {
    if (isNaN(inputValue)) return;
    const newHeap = [...heap, inputValue];
    let curr = newHeap.length - 1;
    setHighlightIdx(curr);
    setActionLog(`Inserted ₹${inputValue} at leaf index ${curr}. Starting bubble-up (heapify-up)...`);

    while (curr > 0 && compare(newHeap[curr], newHeap[parent(curr)])) {
      const p = parent(curr);
      const temp = newHeap[curr];
      newHeap[curr] = newHeap[p];
      newHeap[p] = temp;
      curr = p;
    }

    setHeap([...newHeap]);
    setHighlightIdx(curr);
    setActionLog(`₹${inputValue} settled at index ${curr}. Binary ${heapType}-Heap property satisfied in O(log N)!`);
  };

  const handleExtract = () => {
    if (heap.length === 0) return;
    const root = heap[0];
    if (heap.length === 1) {
      setHeap([]);
      setActionLog(`Extracted root ₹${root}. Heap is now empty.`);
      return;
    }

    const newHeap = [...heap];
    const last = newHeap.pop()!;
    newHeap[0] = last;
    let curr = 0;
    setHighlightIdx(curr);
    setActionLog(`Extracted root ₹${root}. Moved last element ₹${last} to root. Starting bubble-down (heapify-down)...`);

    while (leftChild(curr) < newHeap.length) {
      let candidate = leftChild(curr);
      const right = rightChild(curr);
      if (right < newHeap.length && compare(newHeap[right], newHeap[candidate])) {
        candidate = right;
      }
      if (compare(newHeap[candidate], newHeap[curr])) {
        const temp = newHeap[curr];
        newHeap[curr] = newHeap[candidate];
        newHeap[candidate] = temp;
        curr = candidate;
      } else {
        break;
      }
    }

    setHeap([...newHeap]);
    setHighlightIdx(curr);
    setActionLog(`Extracted top ₹${root}. Heap rebalanced in O(log N) operations.`);
  };

  const handleReset = () => {
    setHeap([150, 110, 130, 80, 95, 105, 120]);
    setHighlightIdx(null);
    setActionLog('Reset to default sample tree.');
  };

  return (
    <div className="p-6 rounded-2xl glass-card border border-slate-800/80 shadow-2xl space-y-6">
      {/* Title & Mode Switch */}
      <div className="flex flex-wrap items-center justify-between gap-4 pb-4 border-b border-slate-800">
        <div>
          <div className="flex items-center gap-2">
            <Cpu className="w-5 h-5 text-blue-400" />
            <h3 className="font-bold text-base text-slate-100">Binary Heap Interactive Visualizer</h3>
            <span className="text-xs font-mono px-2 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
              Order Book Engine
            </span>
          </div>
          <p className="text-xs text-slate-400 mt-1">
            Max-Heap powers Buy Bids (highest price prioritized); Min-Heap powers Sell Asks (lowest ask prioritized).
          </p>
        </div>

        <div className="flex items-center gap-2 p-1 rounded-xl bg-slate-900 border border-slate-800">
          <button
            onClick={() => { setHeapType('MAX'); handleReset(); }}
            className={`px-3 py-1.5 rounded-lg text-xs font-bold font-mono transition-all ${
              heapType === 'MAX'
                ? 'bg-emerald-600 text-white shadow-md'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Max-Heap (Buy Bids)
          </button>
          <button
            onClick={() => { setHeapType('MIN'); handleReset(); }}
            className={`px-3 py-1.5 rounded-lg text-xs font-bold font-mono transition-all ${
              heapType === 'MIN'
                ? 'bg-rose-600 text-white shadow-md'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Min-Heap (Sell Asks)
          </button>
        </div>
      </div>

      {/* Control Bar */}
      <div className="flex flex-wrap items-center gap-3 bg-slate-900/60 p-3 rounded-xl border border-slate-800">
        <div className="flex items-center gap-2">
          <span className="text-xs font-mono text-slate-400">Price: ₹</span>
          <input
            type="number"
            value={inputValue}
            onChange={(e) => setInputValue(parseInt(e.target.value, 10) || 0)}
            className="w-24 px-3 py-1.5 rounded-lg bg-slate-800 border border-slate-700 text-xs font-mono text-slate-100 focus:outline-none focus:border-blue-500"
          />
        </div>

        <button
          onClick={handleInsert}
          className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-blue-600 hover:bg-blue-500 text-white text-xs font-semibold shadow-sm transition-colors"
        >
          <Plus className="w-3.5 h-3.5" />
          <span>Push (Insert)</span>
        </button>

        <button
          onClick={handleExtract}
          disabled={heap.length === 0}
          className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-rose-600 hover:bg-rose-500 text-white text-xs font-semibold shadow-sm transition-colors disabled:opacity-50"
        >
          <Trash2 className="w-3.5 h-3.5" />
          <span>Pop (Extract {heapType === 'MAX' ? 'Max' : 'Min'})</span>
        </button>

        <button
          onClick={handleReset}
          className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-slate-800 hover:bg-slate-700 text-slate-300 text-xs font-semibold border border-slate-700 transition-colors ml-auto"
        >
          <RotateCcw className="w-3.5 h-3.5" />
          <span>Reset</span>
        </button>
      </div>

      {/* Action Log Box */}
      <div className="p-3 rounded-xl bg-black/40 border border-slate-800/80 font-mono text-xs text-slate-300 flex items-center gap-2">
        <span className="text-blue-400 font-bold">[C++ Step]:</span>
        <span>{actionLog}</span>
      </div>

      {/* Tree Visualization Canvas */}
      <div className="min-h-56 p-6 rounded-xl bg-[#060a14] border border-slate-900 flex flex-col items-center justify-center space-y-6">
        {heap.length === 0 ? (
          <div className="text-slate-500 text-xs font-mono">Heap is currently empty. Insert values above.</div>
        ) : (
          <div className="w-full flex flex-col items-center space-y-6">
            {/* Level 0 (Root) */}
            <div className="flex justify-center">
              <HeapNode value={heap[0]} idx={0} isHighlighted={highlightIdx === 0} heapType={heapType} />
            </div>

            {/* Level 1 */}
            {heap.length > 1 && (
              <div className="w-full max-w-sm flex justify-around">
                {heap.slice(1, 3).map((val, i) => (
                  <HeapNode key={i + 1} value={val} idx={i + 1} isHighlighted={highlightIdx === i + 1} heapType={heapType} />
                ))}
              </div>
            )}

            {/* Level 2 */}
            {heap.length > 3 && (
              <div className="w-full max-w-md flex justify-between">
                {heap.slice(3, 7).map((val, i) => (
                  <HeapNode key={i + 3} value={val} idx={i + 3} isHighlighted={highlightIdx === i + 3} heapType={heapType} />
                ))}
              </div>
            )}
          </div>
        )}
      </div>

      {/* Array Representation */}
      <div className="space-y-2">
        <div className="flex items-center justify-between text-xs font-mono text-slate-400">
          <span>Continuous Memory Array Vector (C++ std::vector layout):</span>
          <span>Size: {heap.length}</span>
        </div>
        <div className="flex flex-wrap gap-1.5 p-3 rounded-xl bg-slate-900 border border-slate-800 font-mono text-xs">
          {heap.map((val, idx) => (
            <div
              key={idx}
              className={`px-3 py-1.5 rounded-lg border text-center transition-all ${
                highlightIdx === idx
                  ? 'bg-blue-600/30 border-blue-400 text-blue-300 font-bold scale-105'
                  : 'bg-slate-800/80 border-slate-700 text-slate-300'
              }`}
            >
              <div className="text-[9px] text-slate-500">idx [{idx}]</div>
              <div className="font-bold text-slate-100">₹{val}</div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

const HeapNode: React.FC<{ value: number; idx: number; isHighlighted: boolean; heapType: string }> = ({
  value, idx, isHighlighted, heapType
}) => {
  return (
    <div
      className={`relative w-12 h-12 rounded-xl flex flex-col items-center justify-center font-mono transition-all duration-300 shadow-lg ${
        isHighlighted
          ? 'bg-blue-600 text-white ring-4 ring-blue-500/40 scale-110'
          : heapType === 'MAX'
          ? 'bg-emerald-950/80 border border-emerald-500/50 text-emerald-300'
          : 'bg-rose-950/80 border border-rose-500/50 text-rose-300'
      }`}
    >
      <span className="text-[8px] opacity-70">[{idx}]</span>
      <span className="font-bold text-xs">₹{value}</span>
    </div>
  );
};