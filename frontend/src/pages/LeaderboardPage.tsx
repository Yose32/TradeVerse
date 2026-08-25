import React, { useState, useEffect } from 'react';
import { Award, Trophy, Medal, TrendingUp, Sparkles } from 'lucide-react';
import { api } from '../services/api';
import { LeaderboardUser } from '../types';

export const LeaderboardPage: React.FC = () => {
  const [users, setUsers] = useState<LeaderboardUser[]>([]);

  const fetchLeaderboard = async () => {
    try {
      const data = await api.getLeaderboard();
      setUsers(data);
    } catch (e) {
      console.error(e);
    }
  };

  useEffect(() => {
    fetchLeaderboard();
    const interval = setInterval(fetchLeaderboard, 2000);
    return () => clearInterval(interval);
  }, []);

  const formatINR = (val: number) => {
    return new Intl.NumberFormat('en-IN', {
      style: 'currency',
      currency: 'INR',
      maximumFractionDigits: 0,
    }).format(val);
  };

  const getRankBadge = (rank: number) => {
    if (rank === 1) return <Trophy className="w-5 h-5 text-amber-400" />;
    if (rank === 2) return <Medal className="w-5 h-5 text-slate-300" />;
    if (rank === 3) return <Medal className="w-5 h-5 text-amber-600" />;
    return <span className="font-mono font-bold text-slate-400">#{rank}</span>;
  };

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      {/* Header */}
      <div className="flex flex-wrap items-center justify-between gap-4">
        <div>
          <div className="flex items-center gap-2">
            <h1 className="text-2xl font-extrabold text-slate-100">Trading Competition Leaderboard</h1>
            <span className="text-xs font-mono px-2 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
              Sorted via C++ MergeSort
            </span>
          </div>
          <p className="text-xs text-slate-400 mt-0.5">
            All users started with ₹10,00,000 virtual capital. Ranked in real-time by total portfolio net worth.
          </p>
        </div>
      </div>

      {/* Top 3 Podium Cards */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-4 pt-2">
        {users.slice(0, 3).map((u, i) => (
          <div
            key={u.userId}
            className={`p-6 rounded-2xl glass-card border shadow-xl flex flex-col justify-between space-y-4 ${
              u.rank === 1
                ? 'border-amber-500/30 bg-amber-950/10 shadow-amber-500/5 ring-1 ring-amber-500/20'
                : 'border-slate-800'
            }`}
          >
            <div className="flex items-center justify-between">
              <div className="flex items-center gap-2.5">
                <div className="w-10 h-10 rounded-xl bg-slate-900 border border-slate-700 flex items-center justify-center">
                  {getRankBadge(u.rank)}
                </div>
                <div>
                  <h3 className="font-bold text-sm text-slate-100">{u.fullName}</h3>
                  <p className="text-xs text-slate-400 font-mono">@{u.username}</p>
                </div>
              </div>
              <span className="text-xs font-mono font-bold px-2 py-1 rounded bg-slate-900 border border-slate-700 text-slate-300">
                Rank #{u.rank}
              </span>
            </div>

            <div className="space-y-1 font-mono">
              <div className="text-2xl font-extrabold text-slate-100">
                {formatINR(u.portfolioValue)}
              </div>
              <div className="flex items-center gap-2 text-xs">
                <span className={`font-bold ${u.totalPnL >= 0 ? 'text-emerald-400' : 'text-rose-400'}`}>
                  {u.totalPnL >= 0 ? '+' : ''}{formatINR(u.totalPnL)}
                </span>
                <span className={`font-semibold ${u.returnPercent >= 0 ? 'text-emerald-400' : 'text-rose-400'}`}>
                  ({u.returnPercent >= 0 ? '+' : ''}{u.returnPercent.toFixed(2)}%)
                </span>
              </div>
            </div>
          </div>
        ))}
      </div>

      {/* Full Leaderboard Table */}
      <div className="rounded-2xl glass-card border border-slate-800/80 shadow-2xl overflow-hidden p-5 space-y-3">
        <h3 className="font-bold text-sm text-slate-100 flex items-center gap-2">
          <Award className="w-4 h-4 text-amber-400" />
          <span>Full Standings</span>
        </h3>

        <div className="overflow-x-auto">
          <table className="w-full text-left font-mono text-xs">
            <thead>
              <tr className="border-b border-slate-800 bg-slate-900/60 text-slate-400 uppercase text-[10px] tracking-wider">
                <th className="py-3 px-4">Rank</th>
                <th className="py-3 px-4">Trader</th>
                <th className="py-3 px-4 text-right">Portfolio Net Worth</th>
                <th className="py-3 px-4 text-right">Profit / Loss (₹)</th>
                <th className="py-3 px-4 text-right">Total Return (%)</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-800/60">
              {users.map((u) => {
                const isProfit = u.totalPnL >= 0;
                return (
                  <tr key={u.userId} className="hover:bg-slate-800/40 transition-colors">
                    <td className="py-3.5 px-4 font-bold text-slate-300">
                      <div className="flex items-center gap-2">
                        {getRankBadge(u.rank)}
                      </div>
                    </td>
                    <td className="py-3.5 px-4 font-sans font-medium text-slate-100">
                      <div>
                        <span>{u.fullName}</span>
                        <span className="text-slate-400 font-mono text-[11px] ml-2">@{u.username}</span>
                      </div>
                    </td>
                    <td className="py-3.5 px-4 text-right font-bold text-slate-100">
                      {formatINR(u.portfolioValue)}
                    </td>
                    <td className={`py-3.5 px-4 text-right font-bold ${isProfit ? 'text-emerald-400' : 'text-rose-400'}`}>
                      {isProfit ? '+' : ''}{formatINR(u.totalPnL)}
                    </td>
                    <td className={`py-3.5 px-4 text-right font-bold ${isProfit ? 'text-emerald-400' : 'text-rose-400'}`}>
                      {isProfit ? '+' : ''}{u.returnPercent.toFixed(2)}%
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