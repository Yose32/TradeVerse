import React from 'react';
import { TrendingUp, TrendingDown } from 'lucide-react';
import { useTrading } from '../../context/TradingContext';
import { Stock } from '../../types';

export const TickerTape: React.FC = () => {
  const { stocks, setSelectedStock } = useTrading();

  if (!stocks || stocks.length === 0) return null;

  return (
    <div className="w-full bg-[#060a14] border-b border-slate-800/60 overflow-hidden py-1.5 select-none">
      <div className="flex animate-marquee whitespace-nowrap gap-8 text-xs font-mono">
        {stocks.concat(stocks).map((stock: Stock, idx: number) => {
          const isUp = stock.change >= 0;
          return (
            <button
              key={`${stock.symbol}-${idx}`}
              onClick={() => setSelectedStock(stock)}
              className="inline-flex items-center gap-2 hover:bg-slate-800/40 px-2 py-0.5 rounded transition-colors"
            >
              <span className="font-bold text-slate-200">{stock.symbol}</span>
              <span className="text-slate-300">₹{stock.currentPrice.toFixed(2)}</span>
              <span
                className={`inline-flex items-center gap-0.5 text-[11px] font-semibold ${
                  isUp ? 'text-emerald-400' : 'text-rose-400'
                }`}
              >
                {isUp ? <TrendingUp className="w-3 h-3" /> : <TrendingDown className="w-3 h-3" />}
                {isUp ? '+' : ''}
                {stock.changePercent.toFixed(2)}%
              </span>
            </button>
          );
        })}
      </div>
    </div>
  );
};