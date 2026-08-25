import React, { useState, useEffect } from 'react';
import { Layers, ArrowUpDown, Cpu } from 'lucide-react';
import { api } from '../../services/api';
import { OrderBookDepth, PriceLevel } from '../../types';

interface OrderBookViewProps {
  symbol: string;
  onPriceSelect?: (price: number) => void;
}

export const OrderBookView: React.FC<OrderBookViewProps> = ({ symbol, onPriceSelect }) => {
  const [depth, setDepth] = useState<OrderBookDepth | null>(null);

  useEffect(() => {
    let isMounted = true;
    const fetchDepth = async () => {
      try {
        const data = await api.getOrderBook(symbol);
        if (isMounted) setDepth(data);
      } catch (err) {
        // silent polling catch
      }
    };

    fetchDepth();
    const interval = setInterval(fetchDepth, 1000);
    return () => {
      isMounted = false;
      clearInterval(interval);
    };
  }, [symbol]);

  const maxVolume = React.useMemo(() => {
    if (!depth) return 1;
    const bidMax = depth.bids.reduce((max, b) => Math.max(max, b.quantity), 0);
    const askMax = depth.asks.reduce((max, a) => Math.max(max, a.quantity), 0);
    return Math.max(bidMax, askMax, 1);
  }, [depth]);

  if (!depth) {
    return (
      <div className="p-6 rounded-2xl glass-card border border-slate-800 flex items-center justify-center text-slate-500 text-xs font-mono">
        Loading C++ Order Book Depth...
      </div>
    );
  }

  return (
    <div className="p-4 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-4">
      {/* Header with C++ DSA badge */}
      <div className="flex items-center justify-between pb-3 border-b border-slate-800/80">
        <div className="flex items-center gap-2">
          <Layers className="w-4 h-4 text-blue-400" />
          <h3 className="font-bold text-sm text-slate-100">Order Book</h3>
          <span className="text-[10px] font-mono px-1.5 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
            {symbol}
          </span>
        </div>
        <div className="flex items-center gap-1.5 text-[11px] font-mono text-purple-400 bg-purple-500/10 px-2 py-0.5 rounded-full border border-purple-500/20">
          <Cpu className="w-3 h-3" />
          <span>Max/Min Heap</span>
        </div>
      </div>

      {/* Table Column Headers */}
      <div className="grid grid-cols-3 text-[11px] font-mono font-semibold uppercase tracking-wider text-slate-400 px-2">
        <span>Price (₹)</span>
        <span className="text-right">Size</span>
        <span className="text-right">Orders</span>
      </div>

      {/* Asks (Sell Orders - MinHeap) - Render in reverse (highest to lowest) */}
      <div className="space-y-0.5">
        {depth.asks.slice(0, 7).reverse().map((ask: PriceLevel, idx: number) => {
          const depthPercent = Math.min(100, (ask.quantity / maxVolume) * 100);
          return (
            <div
              key={`ask-${idx}`}
              onClick={() => onPriceSelect && onPriceSelect(ask.price)}
              className="relative grid grid-cols-3 text-xs font-mono py-1 px-2 rounded cursor-pointer hover:bg-rose-500/10 transition-colors group"
            >
              {/* Depth bar indicator */}
              <div
                className="absolute inset-y-0 right-0 bg-rose-500/10 rounded pointer-events-none transition-all duration-300"
                style={{ width: `${depthPercent}%` }}
              />
              <span className="font-semibold text-rose-400 z-10">
                ₹{ask.price.toFixed(2)}
              </span>
              <span className="text-right text-slate-300 z-10">{ask.quantity}</span>
              <span className="text-right text-slate-400 text-[11px] z-10">
                {ask.orderCount}
              </span>
            </div>
          );
        })}
      </div>

      {/* Current Spread & Last Traded Price Divider */}
      <div className="py-2 px-3 rounded-xl bg-slate-900/90 border border-slate-800 flex items-center justify-between font-mono text-xs">
        <div className="flex items-center gap-2">
          <span className="text-slate-400 text-[11px]">Last:</span>
          <span className="font-bold text-sm text-slate-100">
            ₹{depth.lastPrice.toFixed(2)}
          </span>
        </div>
        <div className="flex items-center gap-1.5 text-slate-400 text-[11px]">
          <ArrowUpDown className="w-3 h-3 text-blue-400" />
          <span>Spread: ₹{depth.spread.toFixed(2)}</span>
        </div>
      </div>

      {/* Bids (Buy Orders - MaxHeap) */}
      <div className="space-y-0.5">
        {depth.bids.slice(0, 7).map((bid: PriceLevel, idx: number) => {
          const depthPercent = Math.min(100, (bid.quantity / maxVolume) * 100);
          return (
            <div
              key={`bid-${idx}`}
              onClick={() => onPriceSelect && onPriceSelect(bid.price)}
              className="relative grid grid-cols-3 text-xs font-mono py-1 px-2 rounded cursor-pointer hover:bg-emerald-500/10 transition-colors group"
            >
              {/* Depth bar indicator */}
              <div
                className="absolute inset-y-0 right-0 bg-emerald-500/10 rounded pointer-events-none transition-all duration-300"
                style={{ width: `${depthPercent}%` }}
              />
              <span className="font-semibold text-emerald-400 z-10">
                ₹{bid.price.toFixed(2)}
              </span>
              <span className="text-right text-slate-300 z-10">{bid.quantity}</span>
              <span className="text-right text-slate-400 text-[11px] z-10">
                {bid.orderCount}
              </span>
            </div>
          );
        })}
      </div>
    </div>
  );
};