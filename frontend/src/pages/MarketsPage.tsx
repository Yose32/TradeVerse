import React, { useState } from 'react';
import { Search, Filter, ArrowUpDown, TrendingUp, TrendingDown, ArrowRight } from 'lucide-react';
import { useTrading } from '../context/TradingContext';
import { Stock } from '../types';

interface MarketsPageProps {
  onSelectStock: (stock: Stock) => void;
}

export const MarketsPage: React.FC<MarketsPageProps> = ({ onSelectStock }) => {
  const { stocks } = useTrading();
  const [search, setSearch] = useState<string>('');
  const [selectedSector, setSelectedSector] = useState<string>('ALL');

  const sectors = ['ALL', ...Array.from(new Set(stocks.map((s) => s.sector)))];

  const filteredStocks = stocks.filter((s) => {
    const matchesSearch =
      s.symbol.toLowerCase().includes(search.toLowerCase()) ||
      s.companyName.toLowerCase().includes(search.toLowerCase());
    const matchesSector = selectedSector === 'ALL' || s.sector === selectedSector;
    return matchesSearch && matchesSector;
  });

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      {/* Header */}
      <div className="flex flex-wrap items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-100">Simulated Markets</h1>
          <p className="text-xs text-slate-400 mt-0.5">
            Browse all 10 virtual equities powered by C++ price simulation and depth books
          </p>
        </div>

        {/* Search Bar */}
        <div className="relative w-72">
          <Search className="w-4 h-4 text-slate-400 absolute left-3 top-2.5" />
          <input
            type="text"
            value={search}
            onChange={(e) => setSearch(e.target.value)}
            placeholder="Search symbol or company..."
            className="w-full pl-9 pr-3 py-2 rounded-xl bg-slate-900 border border-slate-700/80 text-xs font-mono text-slate-100 focus:outline-none focus:border-blue-500"
          />
        </div>
      </div>

      {/* Sector Filter Chips */}
      <div className="flex gap-2 overflow-x-auto pb-1">
        {sectors.map((sec) => (
          <button
            key={sec}
            onClick={() => setSelectedSector(sec)}
            className={`px-3 py-1.5 rounded-xl text-xs font-mono font-semibold transition-all whitespace-nowrap ${
              selectedSector === sec
                ? 'bg-blue-600 text-white shadow-md'
                : 'bg-slate-900 border border-slate-800 text-slate-400 hover:text-slate-200'
            }`}
          >
            {sec}
          </button>
        ))}
      </div>

      {/* Stocks Table */}
      <div className="rounded-2xl glass-card border border-slate-800/80 shadow-2xl overflow-hidden">
        <div className="overflow-x-auto">
          <table className="w-full text-left font-mono text-xs">
            <thead>
              <tr className="border-b border-slate-800 bg-slate-900/60 text-slate-400 uppercase text-[10px] tracking-wider">
                <th className="py-3.5 px-4">Symbol</th>
                <th className="py-3.5 px-4">Company</th>
                <th className="py-3.5 px-4">Sector</th>
                <th className="py-3.5 px-4 text-right">Current Price</th>
                <th className="py-3.5 px-4 text-right">24h Change</th>
                <th className="py-3.5 px-4 text-right">Volume</th>
                <th className="py-3.5 px-4 text-right">Market Cap</th>
                <th className="py-3.5 px-4 text-center">Trade</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-800/60">
              {filteredStocks.map((stock) => {
                const isUp = stock.change >= 0;
                return (
                  <tr
                    key={stock.symbol}
                    onClick={() => onSelectStock(stock)}
                    className="hover:bg-slate-800/40 cursor-pointer transition-colors group"
                  >
                    <td className="py-3 px-4 font-bold text-slate-100">
                      <span className="px-2 py-1 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
                        {stock.symbol}
                      </span>
                    </td>
                    <td className="py-3 px-4 text-slate-200 font-sans font-medium">
                      {stock.companyName}
                    </td>
                    <td className="py-3 px-4 text-slate-400">{stock.sector}</td>
                    <td className="py-3 px-4 text-right font-bold text-slate-100">
                      ₹{stock.currentPrice.toFixed(2)}
                    </td>
                    <td className={`py-3 px-4 text-right font-semibold ${isUp ? 'text-emerald-400' : 'text-rose-400'}`}>
                      <span className="inline-flex items-center gap-1">
                        {isUp ? <TrendingUp className="w-3.5 h-3.5" /> : <TrendingDown className="w-3.5 h-3.5" />}
                        {isUp ? '+' : ''}{stock.changePercent.toFixed(2)}%
                      </span>
                    </td>
                    <td className="py-3 px-4 text-right text-slate-300">
                      {stock.volume.toLocaleString()}
                    </td>
                    <td className="py-3 px-4 text-right text-slate-400">
                      ₹{(stock.marketCap / 1e9).toFixed(1)}B
                    </td>
                    <td className="py-3 px-4 text-center">
                      <button
                        onClick={(e) => {
                          e.stopPropagation();
                          onSelectStock(stock);
                        }}
                        className="p-1.5 rounded-lg bg-blue-600/15 text-blue-400 group-hover:bg-blue-600 group-hover:text-white transition-colors"
                      >
                        <ArrowRight className="w-4 h-4" />
                      </button>
                    </td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
};