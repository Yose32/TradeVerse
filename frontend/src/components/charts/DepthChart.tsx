import React, { useState, useEffect } from 'react';
import { api } from '../../services/api';
import { OrderBookDepth } from '../../types';

interface DepthChartProps {
  symbol: string;
}

export const DepthChart: React.FC<DepthChartProps> = ({ symbol }) => {
  const [depth, setDepth] = useState<OrderBookDepth | null>(null);

  useEffect(() => {
    let isMounted = true;
    const fetchDepth = async () => {
      try {
        const data = await api.getOrderBook(symbol);
        if (isMounted) setDepth(data);
      } catch (e) {}
    };
    fetchDepth();
    const interval = setInterval(fetchDepth, 1500);
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [symbol]);

  if (!depth || (depth.bids.length === 0 && depth.asks.length === 0)) {
    return null;
  }

  // Calculate cumulative volumes
  let cumulativeBid = 0;
  const bidPoints = depth.bids.map((b) => {
    cumulativeBid += b.quantity;
    return { price: b.price, cumVol: cumulativeBid };
  });

  let cumulativeAsk = 0;
  const askPoints = depth.asks.map((a) => {
    cumulativeAsk += a.quantity;
    return { price: a.price, cumVol: cumulativeAsk };
  });

  const maxTotal = Math.max(cumulativeBid, cumulativeAsk, 1);

  return (
    <div className="p-4 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-3">
      <div className="flex items-center justify-between text-xs font-mono">
        <span className="font-bold text-slate-300">Market Depth Curve (Bids vs Asks)</span>
        <span className="text-slate-400">Total Volume: {cumulativeBid + cumulativeAsk}</span>
      </div>

      <div className="grid grid-cols-2 gap-3 h-24 text-xs font-mono">
        {/* Bid Depth Side */}
        <div className="bg-emerald-500/5 rounded-xl border border-emerald-500/10 p-2 flex flex-col justify-between">
          <div className="text-[11px] text-emerald-400 font-semibold">Buy Wall (MaxHeap)</div>
          <div className="text-right text-lg font-bold text-emerald-400 font-mono">
            {cumulativeBid} <span className="text-[10px] text-slate-400">shares</span>
          </div>
          <div className="w-full bg-slate-800/60 rounded-full h-1.5 overflow-hidden">
            <div
              className="bg-emerald-500 h-full transition-all duration-300"
              style={{ width: `${(cumulativeBid / maxTotal) * 100}%` }}
            />
          </div>
        </div>

        {/* Ask Depth Side */}
        <div className="bg-rose-500/5 rounded-xl border border-rose-500/10 p-2 flex flex-col justify-between">
          <div className="text-[11px] text-rose-400 font-semibold">Sell Wall (MinHeap)</div>
          <div className="text-right text-lg font-bold text-rose-400 font-mono">
            {cumulativeAsk} <span className="text-[10px] text-slate-400">shares</span>
          </div>
          <div className="w-full bg-slate-800/60 rounded-full h-1.5 overflow-hidden">
            <div
              className="bg-rose-500 h-full transition-all duration-300"
              style={{ width: `${(cumulativeAsk / maxTotal) * 100}%` }}
            />
          </div>
        </div>
      </div>
    </div>
  );
};