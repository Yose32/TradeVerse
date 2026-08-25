import React, { useState, useEffect } from 'react';
import { History, ArrowUpRight, ArrowDownRight, Search } from 'lucide-react';
import { api } from '../services/api';
import { Trade } from '../types';

export const HistoryPage: React.FC = () => {
  const [trades, setTrades] = useState<Trade[]>([]);
  const [search, setSearch] = useState<string>('');

  useEffect(() => {
    api.getTrades().then(setTrades).catch(console.error);
    const interval = setInterval(() => {
      api.getTrades().then(setTrades).catch(console.error);
    }, 2000);
    return () => clearInterval(interval);
  }, []);

  const filteredTrades = trades.filter((t) =>
    t.symbol.toLowerCase().includes(search.toLowerCase()) ||
    t.tradeId.toLowerCase().includes(search.toLowerCase())
  );

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      <div className="flex flex-wrap items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-100">Transaction & Trade History</h1>
          <p className="text-xs text-slate-400 mt-0.5">
            Immutable trade ledger executed by C++ matching engine and recorded in SQLite
          </p>
        </div>

        <div className="relative w-64">
          <Search className="w-4 h-4 text-slate-400 absolute left-3 top-2.5" />
          <input
            type="text"
            value={search}
            onChange={(e) => setSearch(e.target.value)}
            placeholder="Search symbol or trade ID..."
            className="w-full pl-9 pr-3 py-2 rounded-xl bg-slate-900 border border-slate-700/80 text-xs font-mono text-slate-100 focus:outline-none focus:border-blue-500"
          />
        </div>
      </div>

      <div className="rounded-2xl glass-card border border-slate-800/80 shadow-2xl overflow-hidden p-5 space-y-3">
        {filteredTrades.length === 0 ? (
          <div className="py-12 text-center text-slate-500 font-mono text-xs">
            No executed trades recorded yet.
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left font-mono text-xs">
              <thead>
                <tr className="border-b border-slate-800 bg-slate-900/60 text-slate-400 uppercase text-[10px] tracking-wider">
                  <th className="py-3 px-3">Trade ID</th>
                  <th className="py-3 px-3">Symbol</th>
                  <th className="py-3 px-3">Side</th>
                  <th className="py-3 px-3 text-right">Quantity</th>
                  <th className="py-3 px-3 text-right">Execution Price</th>
                  <th className="py-3 px-3 text-right">Total Trade Value</th>
                  <th className="py-3 px-3 text-right">Execution Time</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-800/60">
                {filteredTrades.map((t) => {
                  const isBuy = t.side === 'BUY';
                  return (
                    <tr key={t.tradeId} className="hover:bg-slate-800/40 transition-colors">
                      <td className="py-3 px-3 font-bold text-slate-300">{t.tradeId}</td>
                      <td className="py-3 px-3 font-bold text-slate-100">{t.symbol}</td>
                      <td className="py-3 px-3 font-bold">
                        <span className={`px-2 py-0.5 rounded text-[11px] flex items-center gap-1 w-max ${
                          isBuy ? 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/20' : 'bg-rose-500/10 text-rose-400 border border-rose-500/20'
                        }`}>
                          {isBuy ? <ArrowUpRight className="w-3 h-3" /> : <ArrowDownRight className="w-3 h-3" />}
                          {t.side}
                        </span>
                      </td>
                      <td className="py-3 px-3 text-right text-slate-200">{t.quantity}</td>
                      <td className="py-3 px-3 text-right font-bold text-slate-100">₹{t.price.toFixed(2)}</td>
                      <td className="py-3 px-3 text-right text-slate-200 font-bold">₹{t.totalValue.toFixed(2)}</td>
                      <td className="py-3 px-3 text-right text-slate-400 text-[11px]">
                        {new Date(t.timestamp).toLocaleTimeString()}
                      </td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
          </div>
        )}
      </div>
    </div>
  );
};