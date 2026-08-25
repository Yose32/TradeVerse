import React, { useState, useEffect } from 'react';
import { Network, Play, RefreshCw, Cpu, GitBranch, Compass } from 'lucide-react';
import { api } from '../../services/api';
import { MarketGraphData, DijkstraResult } from '../../types';

export const GraphVisualizer: React.FC = () => {
  const [graphData, setGraphData] = useState<MarketGraphData | null>(null);
  const [algorithm, setAlgorithm] = useState<'DIJKSTRA' | 'BFS' | 'DFS'>('DIJKSTRA');
  const [sourceNode, setSourceNode] = useState<string>('Semiconductors');
  const [targetNode, setTargetNode] = useState<string>('MSFT');
  const [dijkstraResult, setDijkstraResult] = useState<DijkstraResult | null>(null);
  const [bfsResult, setBfsResult] = useState<any | null>(null);
  const [isRunning, setIsRunning] = useState<boolean>(false);

  useEffect(() => {
    api.getMarketGraph().then(setGraphData).catch(console.error);
  }, []);

  const handleRunAlgorithm = async () => {
    setIsRunning(true);
    try {
      if (algorithm === 'DIJKSTRA') {
        const res = await api.runDijkstra(sourceNode, targetNode);
        setDijkstraResult(res);
        setBfsResult(null);
      } else if (algorithm === 'BFS') {
        const res = await api.runBFS(sourceNode);
        setBfsResult(res);
        setDijkstraResult(null);
      } else {
        const res = await api.runDFS(sourceNode);
        setBfsResult(res);
        setDijkstraResult(null);
      }
    } catch (e) {
      console.error(e);
    } finally {
      setIsRunning(false);
    }
  };

  const isPathNode = (node: string) => {
    if (dijkstraResult && dijkstraResult.path.includes(node)) return true;
    return false;
  };

  return (
    <div className="p-6 rounded-2xl glass-card border border-slate-800/80 shadow-2xl space-y-6">
      {/* Title */}
      <div className="flex flex-wrap items-center justify-between gap-4 pb-4 border-b border-slate-800">
        <div>
          <div className="flex items-center gap-2">
            <Network className="w-5 h-5 text-purple-400" />
            <h3 className="font-bold text-base text-slate-100">Market Dependency Graph & Shockwave Engine</h3>
            <span className="text-xs font-mono px-2 py-0.5 rounded bg-purple-500/10 text-purple-400 border border-purple-500/20">
              Graph DSA Core
            </span>
          </div>
          <p className="text-xs text-slate-400 mt-1">
            Models supply chain linkages, correlated asset contagion, and sector propagation paths in C++.
          </p>
        </div>

        {/* Algorithm Toggle */}
        <div className="flex items-center gap-1.5 p-1 rounded-xl bg-slate-900 border border-slate-800">
          {(['DIJKSTRA', 'BFS', 'DFS'] as const).map((algo) => (
            <button
              key={algo}
              onClick={() => setAlgorithm(algo)}
              className={`px-3 py-1.5 rounded-lg text-xs font-mono font-bold transition-all ${
                algorithm === algo
                  ? 'bg-purple-600 text-white shadow-md'
                  : 'text-slate-400 hover:text-slate-200'
              }`}
            >
              {algo}
            </button>
          ))}
        </div>
      </div>

      {/* Parameter Inputs & Run Button */}
      <div className="flex flex-wrap items-center gap-4 bg-slate-900/60 p-4 rounded-xl border border-slate-800">
        <div className="flex items-center gap-2">
          <span className="text-xs font-mono text-slate-400">Epicentre / Source:</span>
          <select
            value={sourceNode}
            onChange={(e) => setSourceNode(e.target.value)}
            className="px-3 py-1.5 rounded-lg bg-slate-800 border border-slate-700 text-xs font-mono text-slate-100 focus:outline-none focus:border-purple-500"
          >
            {graphData?.vertices.map((v) => (
              <option key={v} value={v}>{v}</option>
            ))}
          </select>
        </div>

        {algorithm === 'DIJKSTRA' && (
          <div className="flex items-center gap-2">
            <span className="text-xs font-mono text-slate-400">Target Asset:</span>
            <select
              value={targetNode}
              onChange={(e) => setTargetNode(e.target.value)}
              className="px-3 py-1.5 rounded-lg bg-slate-800 border border-slate-700 text-xs font-mono text-slate-100 focus:outline-none focus:border-purple-500"
            >
              {graphData?.vertices.map((v) => (
                <option key={v} value={v}>{v}</option>
              ))}
            </select>
          </div>
        )}

        <button
          onClick={handleRunAlgorithm}
          disabled={isRunning}
          className="flex items-center gap-2 px-4 py-1.5 rounded-lg bg-purple-600 hover:bg-purple-500 text-white text-xs font-bold shadow-lg shadow-purple-500/20 transition-all ml-auto"
        >
          {isRunning ? <RefreshCw className="w-4 h-4 animate-spin" /> : <Play className="w-4 h-4" />}
          <span>Run {algorithm} in C++</span>
        </button>
      </div>

      {/* Results Banner */}
      {dijkstraResult && (
        <div className="p-4 rounded-xl bg-purple-950/40 border border-purple-500/30 space-y-2">
          <div className="flex items-center justify-between text-xs font-mono">
            <span className="font-bold text-purple-300">
              Dijkstra Shortest Propagation Path:
            </span>
            <span className="text-purple-400 font-bold">
              Total Resistance Distance: {dijkstraResult.distance.toFixed(2)}
            </span>
          </div>
          <div className="flex items-center gap-2 flex-wrap text-xs font-mono">
            {dijkstraResult.path.map((node, i) => (
              <React.Fragment key={node}>
                <span className="px-2.5 py-1 rounded-lg bg-purple-600/30 border border-purple-400 text-purple-200 font-bold">
                  {node}
                </span>
                {i + 1 < dijkstraResult.path.length && (
                  <span className="text-purple-400 font-bold">→</span>
                )}
              </React.Fragment>
            ))}
          </div>
        </div>
      )}

      {bfsResult && (
        <div className="p-4 rounded-xl bg-blue-950/40 border border-blue-500/30 space-y-2">
          <div className="text-xs font-mono font-bold text-blue-300">
            {bfsResult.algorithm} Contagion Traversal Sequence:
          </div>
          <div className="flex items-center gap-2 flex-wrap text-xs font-mono">
            {bfsResult.traversal.map((node: string, i: number) => (
              <span key={node} className="px-2 py-1 rounded-md bg-blue-600/20 border border-blue-500/40 text-blue-200">
                {i + 1}. {node}
              </span>
            ))}
          </div>
        </div>
      )}

      {/* Network Nodes Grid */}
      <div className="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 gap-3">
        {graphData?.vertices.map((vertex) => {
          const isSource = vertex === sourceNode;
          const isTarget = vertex === targetNode && algorithm === 'DIJKSTRA';
          const inPath = isPathNode(vertex);

          return (
            <div
              key={vertex}
              className={`p-3 rounded-xl border text-center transition-all ${
                isSource
                  ? 'bg-purple-600 text-white font-bold ring-2 ring-purple-400'
                  : isTarget
                  ? 'bg-blue-600 text-white font-bold ring-2 ring-blue-400'
                  : inPath
                  ? 'bg-purple-950/60 border-purple-500/80 text-purple-300 font-bold shadow-md'
                  : 'bg-slate-900/80 border-slate-800 text-slate-300'
              }`}
            >
              <div className="text-[10px] opacity-75 font-mono">
                {isSource ? 'SOURCE EPICENTRE' : isTarget ? 'TARGET ASSET' : inPath ? 'ON PATH' : 'NODE'}
              </div>
              <div className="font-mono text-sm">{vertex}</div>
            </div>
          );
        })}
      </div>
    </div>
  );
};