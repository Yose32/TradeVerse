import React from 'react';
import {
  TrendingUp, Shield, Cpu, Award, PieChart, Clock,
  Bot, Sliders, LogOut, User as UserIcon, LogIn, Activity
} from 'lucide-react';
import { useAuth } from '../../context/AuthContext';
import { useTrading } from '../../context/TradingContext';

interface NavbarProps {
  activeTab: string;
  setActiveTab: (tab: string) => void;
  onOpenAI: () => void;
  onOpenAdmin: () => void;
}

export const Navbar: React.FC<NavbarProps> = ({
  activeTab,
  setActiveTab,
  onOpenAI,
  onOpenAdmin,
}) => {
  const { user, isAuthenticated, logout } = useAuth();
  const { portfolio } = useTrading();

  const navItems = [
    { id: 'dashboard', label: 'Dashboard', icon: TrendingUp },
    { id: 'markets', label: 'Markets', icon: Activity },
    { id: 'portfolio', label: 'Portfolio', icon: PieChart },
    { id: 'orders', label: 'Orders', icon: Clock },
    { id: 'leaderboard', label: 'Leaderboard', icon: Award },
    { id: 'dsa_lab', label: 'DSA Lab', icon: Cpu, badge: 'Academic Core' },
  ];

  const formatINR = (val: number) => {
    return new Intl.NumberFormat('en-IN', {
      style: 'currency',
      currency: 'INR',
      maximumFractionDigits: 0,
    }).format(val);
  };

  return (
    <header className="sticky top-0 z-40 w-full bg-[#0b1329]/90 backdrop-blur-md border-b border-slate-800/80">
      <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
        <div className="flex items-center justify-between h-16">
          {/* Logo & Platform Title */}
          <div className="flex items-center gap-6">
            <button
              onClick={() => setActiveTab('dashboard')}
              className="flex items-center gap-3 group text-left"
            >
              <div className="w-10 h-10 rounded-xl bg-gradient-to-tr from-brand to-accent flex items-center justify-center shadow-lg shadow-blue-500/20 group-hover:scale-105 transition-transform">
                <TrendingUp className="w-5 h-5 text-white" />
              </div>
              <div>
                <div className="flex items-center gap-2">
                  <span className="font-extrabold text-lg tracking-tight bg-gradient-to-r from-blue-400 via-indigo-300 to-purple-400 bg-clip-text text-transparent">
                    TradeVerse
                  </span>
                  <span className="text-[10px] font-mono uppercase tracking-wider px-1.5 py-0.5 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
                    C++ DSA Core
                  </span>
                </div>
                <p className="text-[11px] text-slate-400 font-medium">
                  Paper Trading Simulator
                </p>
              </div>
            </button>

            {/* Navigation Tabs */}
            <nav className="hidden md:flex items-center gap-1">
              {navItems.map((item) => {
                const Icon = item.icon;
                const isActive = activeTab === item.id;
                return (
                  <button
                    key={item.id}
                    onClick={() => setActiveTab(item.id)}
                    className={`flex items-center gap-2 px-3.5 py-2 rounded-lg text-sm font-medium transition-all ${
                      isActive
                        ? 'bg-blue-600/15 text-blue-400 border border-blue-500/30 shadow-sm'
                        : 'text-slate-400 hover:text-slate-200 hover:bg-slate-800/50'
                    }`}
                  >
                    <Icon className="w-4 h-4" />
                    <span>{item.label}</span>
                    {item.badge && (
                      <span className="text-[9px] uppercase font-bold tracking-wider px-1.5 py-0.2 rounded-full bg-accent/20 text-purple-300 border border-accent/30">
                        {item.badge}
                      </span>
                    )}
                  </button>
                );
              })}
            </nav>
          </div>

          {/* Right Action Bar */}
          <div className="flex items-center gap-3">
            {/* Simulation Status Badge */}
            <div className="hidden lg:flex items-center gap-2 px-2.5 py-1 rounded-full bg-emerald-500/10 border border-emerald-500/20 text-emerald-400 text-xs font-mono">
              <span className="w-2 h-2 rounded-full bg-emerald-400 animate-ping" />
              <span>SIMULATED MARKET LIVE</span>
            </div>

            {/* AI Assistant Button */}
            <button
              onClick={onOpenAI}
              className="flex items-center gap-2 px-3 py-1.5 rounded-lg text-xs font-semibold bg-gradient-to-r from-purple-600/20 to-indigo-600/20 text-purple-300 border border-purple-500/30 hover:bg-purple-600/30 transition-colors shadow-sm"
              title="TradeVerse AI Assistant"
            >
              <Bot className="w-4 h-4 text-purple-400" />
              <span className="hidden sm:inline">AI Companion</span>
            </button>

            {/* Admin / Demo Controller Button */}
            <button
              onClick={onOpenAdmin}
              className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-semibold bg-slate-800/80 text-slate-300 border border-slate-700 hover:bg-slate-700 transition-colors"
              title="Simulation & Market Regime Controller"
            >
              <Sliders className="w-4 h-4 text-slate-400" />
              <span className="hidden sm:inline">Sim Controls</span>
            </button>

            {/* User Account / Balance Widget */}
            {isAuthenticated && user ? (
              <div className="flex items-center gap-3 pl-2 border-l border-slate-800">
                <div className="text-right hidden sm:block">
                  <div className="text-xs text-slate-400 font-medium">
                    {user.fullName}
                  </div>
                  <div className="text-xs font-mono font-bold text-emerald-400">
                    {formatINR(portfolio?.totalPortfolioValue || user.cashBalance)}
                  </div>
                </div>

                <div className="w-8 h-8 rounded-full bg-gradient-to-tr from-blue-600 to-indigo-600 flex items-center justify-center font-bold text-xs text-white shadow-md">
                  {user.username.substring(0, 2).toUpperCase()}
                </div>

                <button
                  onClick={logout}
                  className="p-2 rounded-lg text-slate-400 hover:text-rose-400 hover:bg-rose-500/10 transition-colors"
                  title="Log Out"
                >
                  <LogOut className="w-4 h-4" />
                </button>
              </div>
            ) : (
              <div className="flex items-center gap-2">
                <button
                  onClick={() => setActiveTab('login')}
                  className="flex items-center gap-1.5 px-3.5 py-1.5 rounded-lg text-xs font-semibold bg-blue-600 text-white hover:bg-blue-500 transition-colors shadow-lg shadow-blue-500/25"
                >
                  <LogIn className="w-3.5 h-3.5" />
                  <span>Log In</span>
                </button>
              </div>
            )}
          </div>
        </div>
      </div>
    </header>
  );
};