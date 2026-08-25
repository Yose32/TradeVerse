import React from 'react';
import {
  Wallet, DollarSign, TrendingUp, TrendingDown, ArrowUpRight, ArrowDownRight,
  Activity, Layers, PieChart, ShieldCheck, Zap
} from 'lucide-react';
import { useAuth } from '../context/AuthContext';
import { useTrading } from '../context/TradingContext';
import { StatCard } from '../components/common/StatCard';
import { CandlestickChart } from '../components/charts/CandlestickChart';
import { OrderBookView } from '../components/trading/OrderBookView';
import { TradeForm } from '../components/trading/TradeForm';
import { DepthChart } from '../components/charts/DepthChart';
import { Stock } from '../types';

interface DashboardPageProps {
  onSelectStock: (stock: Stock) => void;
  onNavigateMarkets: () => void;
}

export const DashboardPage: React.FC<DashboardPageProps> = ({ onSelectStock, onNavigateMarkets }) => {
  const { user } = useAuth();
  const { stocks, portfolio, selectedStock, setSelectedStock } = useTrading();
  const [selectedPrice, setSelectedPrice] = React.useState<number | undefined>(undefined);

  const activeStock = selectedStock || (stocks.length > 0 ? stocks[0] : null);

  const formatINR = (val: number) => {
    return new Intl.NumberFormat('en-IN', {
      style: 'currency',
      currency: 'INR',
      maximumFractionDigits: 2,
    }).format(val);
  };

  // Top gainers and losers
  const sortedStocks = [...stocks].sort((a, b) => b.changePercent - a.changePercent);
  const topGainers = sortedStocks.slice(0, 3);
  const topLosers = [...sortedStocks].reverse().slice(0, 3);

  return (
    <div className="space-y-6 max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 animate-fadeIn">
      {/* Top Stat Cards */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        <StatCard
          title="Total Portfolio Value"
          value={formatINR(portfolio?.totalPortfolioValue || user?.cashBalance || 1000000)}
          change={portfolio?.totalPnL || 0}
          changePercent={portfolio?.returnPercent || 0}
          icon={Wallet}
          variant="brand"
        />
        <StatCard
          title="Virtual Cash Balance"
          value={formatINR(portfolio?.cashBalance || user?.cashBalance || 1000000)}
          subtitle="Available Buying Power"
          icon={DollarSign}
        />
        <StatCard
          title="Total Invested Value"
          value={formatINR(portfolio?.investedValue || 0)}
          subtitle={`Across ${portfolio?.holdings.filter((h) => h.quantity > 0).length || 0} Active Assets`}
          icon={PieChart}
        />
        <StatCard
          title="Unrealized P&L"
          value={formatINR(portfolio?.unrealizedPnL || 0)}
          change={portfolio?.unrealizedPnL || 0}
          icon={Activity}
          variant={(portfolio?.unrealizedPnL || 0) >= 0 ? 'success' : 'danger'}
        />
      </div>

      {/* Main Trading Area Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Left 2 Cols: Candlestick Chart & Depth Chart */}
        <div className="lg:col-span-2 space-y-6">
          {activeStock ? (
            <>
              <CandlestickChart stock={activeStock} />
              <DepthChart symbol={activeStock.symbol} />
            </>
          ) : (
            <div className="p-12 text-center text-slate-500 font-mono text-xs">
              Loading Market Data...
            </div>
          )}
        </div>

        {/* Right Col: Order Book & Buy/Sell Trading Panel */}
        <div className="space-y-6">
          {activeStock && (
            <>
              <TradeForm stock={activeStock} prefillPrice={selectedPrice} />
              <OrderBookView
                symbol={activeStock.symbol}
                onPriceSelect={(p) => setSelectedPrice(p)}
              />
            </>
          )}
        </div>
      </div>

      {/* Bottom Market Overview: Watchlist & Gainers/Losers */}
      <div className="grid grid-cols-1 md:grid-cols-2 gap-6 pt-2">
        {/* Top Market Movers */}
        <div className="p-5 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-4">
          <div className="flex items-center justify-between pb-2 border-b border-slate-800">
            <h3 className="font-bold text-sm text-slate-100 flex items-center gap-2">
              <TrendingUp className="w-4 h-4 text-emerald-400" />
              <span>Top Gainers & Losers (C++ QuickSort)</span>
            </h3>
            <button
              onClick={onNavigateMarkets}
              className="text-xs font-mono text-blue-400 hover:underline"
            >
              View All Markets →
            </button>
          </div>

          <div className="grid grid-cols-2 gap-4">
            {/* Gainers */}
            <div className="space-y-2">
              <span className="text-[11px] font-mono text-slate-400 uppercase tracking-wider font-semibold">
                Gainers
              </span>
              {topGainers.map((s) => (
                <div
                  key={s.symbol}
                  onClick={() => setSelectedStock(s)}
                  className="p-2.5 rounded-xl bg-emerald-950/20 border border-emerald-500/20 flex items-center justify-between cursor-pointer hover:border-emerald-500/40 transition-colors"
                >
                  <div>
                    <div className="font-bold text-xs text-slate-100">{s.symbol}</div>
                    <div className="text-[10px] text-slate-400">₹{s.currentPrice.toFixed(2)}</div>
                  </div>
                  <span className="text-xs font-mono font-bold text-emerald-400 flex items-center gap-0.5">
                    <ArrowUpRight className="w-3.5 h-3.5" />
                    +{s.changePercent.toFixed(2)}%
                  </span>
                </div>
              ))}
            </div>

            {/* Losers */}
            <div className="space-y-2">
              <span className="text-[11px] font-mono text-slate-400 uppercase tracking-wider font-semibold">
                Decliners
              </span>
              {topLosers.map((s) => (
                <div
                  key={s.symbol}
                  onClick={() => setSelectedStock(s)}
                  className="p-2.5 rounded-xl bg-rose-950/20 border border-rose-500/20 flex items-center justify-between cursor-pointer hover:border-rose-500/40 transition-colors"
                >
                  <div>
                    <div className="font-bold text-xs text-slate-100">{s.symbol}</div>
                    <div className="text-[10px] text-slate-400">₹{s.currentPrice.toFixed(2)}</div>
                  </div>
                  <span className="text-xs font-mono font-bold text-rose-400 flex items-center gap-0.5">
                    <ArrowDownRight className="w-3.5 h-3.5" />
                    {s.changePercent.toFixed(2)}%
                  </span>
                </div>
              ))}
            </div>
          </div>
        </div>

        {/* Watchlist Strip */}
        <div className="p-5 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-4">
          <div className="flex items-center justify-between pb-2 border-b border-slate-800">
            <h3 className="font-bold text-sm text-slate-100 flex items-center gap-2">
              <Activity className="w-4 h-4 text-blue-400" />
              <span>Simulated Asset Watchlist</span>
            </h3>
            <span className="text-[10px] font-mono text-slate-400">O(1) Hash Table Lookup</span>
          </div>

          <div className="space-y-2">
            {stocks.slice(0, 4).map((s) => (
              <div
                key={s.symbol}
                onClick={() => setSelectedStock(s)}
                className="p-2.5 rounded-xl bg-slate-900/60 border border-slate-800 flex items-center justify-between cursor-pointer hover:bg-slate-800/50 transition-colors"
              >
                <div>
                  <span className="font-bold text-xs text-slate-100 mr-2">{s.symbol}</span>
                  <span className="text-[11px] text-slate-400">{s.companyName}</span>
                </div>
                <div className="text-right font-mono">
                  <div className="text-xs font-bold text-slate-200">₹{s.currentPrice.toFixed(2)}</div>
                  <div className={`text-[10px] ${s.change >= 0 ? 'text-emerald-400' : 'text-rose-400'}`}>
                    {s.change >= 0 ? '+' : ''}{s.changePercent.toFixed(2)}%
                  </div>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
};