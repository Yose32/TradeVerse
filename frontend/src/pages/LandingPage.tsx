import React from 'react';
import {
  TrendingUp, Shield, Cpu, Award, ArrowRight, Zap, CheckCircle2,
  BarChart2, Layers, Network, Lock, Sparkles
} from 'lucide-react';
import { useAuth } from '../context/AuthContext';

interface LandingPageProps {
  onExplore: () => void;
  onLogin: () => void;
  onSignUp: () => void;
}

export const LandingPage: React.FC<LandingPageProps> = ({ onExplore, onLogin, onSignUp }) => {
  const { login } = useAuth();

  const handleDemoLogin = async () => {
    try {
      await login('demo_trader', 'DemoPass123!');
      onExplore();
    } catch (e) {
      onLogin();
    }
  };

  const features = [
    {
      icon: Layers,
      title: 'Priority Queue Order Book',
      desc: 'Real-time Max-Heap (Bids) and Min-Heap (Asks) matching engine built in C++.',
    },
    {
      icon: Cpu,
      title: 'Price-Time Priority Engine',
      desc: 'Authentic price priority with FIFO execution on ties and partial fill persistence.',
    },
    {
      icon: Network,
      title: 'Graph Market Shockwaves',
      desc: 'Dijkstra and BFS algorithms propagate macroeconomic events across interconnected sectors.',
    },
    {
      icon: Award,
      title: 'Merge Sort Leaderboard',
      desc: 'Stable O(N log N) algorithmic ranking of trading participants by portfolio valuation.',
    },
  ];

  return (
    <div className="min-h-screen flex flex-col justify-between space-y-16 py-12 px-4 sm:px-6 lg:px-8 max-w-7xl mx-auto">
      {/* Hero Section */}
      <div className="text-center space-y-6 pt-8">
        <div className="inline-flex items-center gap-2 px-3 py-1.5 rounded-full bg-blue-500/10 border border-blue-500/20 text-blue-400 text-xs font-mono">
          <Sparkles className="w-3.5 h-3.5" />
          <span>Full-Stack C++ Data Structures & Algorithms Project</span>
        </div>

        <h1 className="text-4xl sm:text-6xl font-extrabold tracking-tight text-slate-100 max-w-4xl mx-auto leading-tight">
          Learn. Trade. Compete.{' '}
          <span className="bg-gradient-to-r from-blue-400 via-indigo-300 to-purple-400 bg-clip-text text-transparent">
            Powered by C++ DSA.
          </span>
        </h1>

        <p className="text-slate-400 text-base sm:text-lg max-w-2xl mx-auto leading-relaxed">
          TradeVerse is a virtual stock-market paper trading simulator where every order match, heap priority queue,
          graph shockwave, and leaderboard sort is computed natively in C++.
        </p>

        {/* Action Buttons */}
        <div className="flex flex-wrap items-center justify-center gap-4 pt-4">
          <button
            onClick={onSignUp}
            className="flex items-center gap-2 px-6 py-3.5 rounded-xl bg-blue-600 hover:bg-blue-500 text-white font-bold text-sm shadow-xl shadow-blue-500/25 transition-all transform hover:-translate-y-0.5"
          >
            <span>Create Free Account</span>
            <ArrowRight className="w-4 h-4" />
          </button>

          <button
            onClick={handleDemoLogin}
            className="flex items-center gap-2 px-6 py-3.5 rounded-xl bg-purple-600/20 hover:bg-purple-600/30 text-purple-300 border border-purple-500/30 font-bold text-sm shadow-lg transition-all"
          >
            <Zap className="w-4 h-4 text-purple-400" />
            <span>Launch Live Demo (₹10,00,000)</span>
          </button>

          <button
            onClick={onExplore}
            className="px-6 py-3.5 rounded-xl bg-slate-800/80 hover:bg-slate-700 text-slate-300 border border-slate-700 font-bold text-sm transition-all"
          >
            Explore Markets
          </button>
        </div>

        <div className="pt-2 flex items-center justify-center gap-6 text-xs text-slate-400 font-mono">
          <div className="flex items-center gap-1.5">
            <CheckCircle2 className="w-4 h-4 text-emerald-400" />
            <span>₹10,00,000 Starting Virtual Capital</span>
          </div>
          <div className="flex items-center gap-1.5">
            <CheckCircle2 className="w-4 h-4 text-emerald-400" />
            <span>No Real Money Required</span>
          </div>
          <div className="flex items-center gap-1.5">
            <CheckCircle2 className="w-4 h-4 text-emerald-400" />
            <span>Live DSA Laboratory</span>
          </div>
        </div>
      </div>

      {/* Feature Cards Grid */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        {features.map((feat, idx) => {
          const Icon = feat.icon;
          return (
            <div
              key={idx}
              className="p-6 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-3 hover:border-slate-700 transition-all group"
            >
              <div className="w-10 h-10 rounded-xl bg-blue-600/15 border border-blue-500/30 flex items-center justify-center text-blue-400 group-hover:scale-110 transition-transform">
                <Icon className="w-5 h-5" />
              </div>
              <h3 className="font-bold text-base text-slate-100">{feat.title}</h3>
              <p className="text-xs text-slate-400 leading-relaxed">{feat.desc}</p>
            </div>
          );
        })}
      </div>

      {/* Footer / Academic Attribution */}
      <div className="text-center py-6 border-t border-slate-800/80 text-xs text-slate-400 font-mono">
        <p>TradeVerse — College Data Structures & Algorithms Examination Project</p>
        <p className="mt-1 text-slate-400">
          Source Engine: C++20 Core | Persistence: SQLite3 WAL | Frontend: React + TypeScript + Vite + Tailwind CSS
        </p>
      </div>
    </div>
  );
};