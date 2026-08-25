import React from 'react';
import { PieChart, TrendingUp, TrendingDown, DollarSign, Wallet, Activity, ArrowRight } from 'lucide-react';
import { useTrading } from '../context/TradingContext';
import { useAuth } from '../context/AuthContext';
import { StatCard } from '../components/common/StatCard';
import { Stock } from '../types';

interface PortfolioPageProps {
  onSelectStock: (stock: Stock) => void;
}

export const PortfolioPage: React.FC<PortfolioPageProps> = ({ onSelectStock }) => {
  const { user } = useAuth();
  const { portfolio, stocks } = useTrading();

  const formatINR = (val: number) => {
    return new Intl.NumberFormat('en-IN', {
      style: 'currency',
      currency: 'INR',
      maximumFractionDigits: 2,
    }).format(val);
  };

  const activeHoldings = portfolio?.holdings.filter((h) => h.quantity > 0) || [];

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      <div>
        <h1 className="text-2xl font-extrabold text-slate-100">Portfolio & Asset Allocation</h1>
        <p className="text-xs text-slate-400 mt-0.5">
          Real-time mark-to-market valuations and realized/unrealized profit & loss calculated by C++
        </p>
      </div>

      {/* Summary Cards */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-4">
        <StatCard
          title="Total Net Worth"
          value={formatINR(portfolio?.totalPortfolioValue || user?.cashBalance || 1000000)}
          change={portfolio?.totalPnL || 0}
          changePercent={portfolio?.returnPercent || 0}
          icon={Wallet}
          variant="brand"
        />
        <StatCard
          title="Liquid Cash"
          value={formatINR(portfolio?.cashBalance || user?.cashBalance || 1000000)}
          subtitle="Available to Trade"
          icon={DollarSign}
        />
        <StatCard
          title="Invested Value"
          value={formatINR(portfolio?.investedValue || 0)}
          subtitle={`${activeHoldings.length} Active Positions`}
          icon={PieChart}
        />
        <StatCard
          title="Realized P&L"
          value={formatINR(portfolio?.realizedPnL || 0)}
          icon={Activity}
          variant={(portfolio?.realizedPnL || 0) >= 0 ? 'success' : 'danger'}
        />
      </div>

      {/* Holdings Table */}
      <div className="rounded-2xl glass-card border border-slate-800/80 shadow-2xl overflow-hidden space-y-3 p-5">
        <h3 className="font-bold text-sm text-slate-100 flex items-center gap-2">
          <PieChart className="w-4 h-4 text-blue-400" />
          <span>Active Holdings ({activeHoldings.length})</span>
        </h3>

        {activeHoldings.length === 0 ? (
          <div className="py-12 text-center text-slate-500 font-mono text-xs">
            No active stock holdings yet. Place a BUY order on any simulated market to start building your portfolio!
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left font-mono text-xs">
              <thead>
                <tr className="border-b border-slate-800 bg-slate-900/60 text-slate-400 uppercase text-[10px] tracking-wider">
                  <th className="py-3 px-3">Symbol</th>
                  <th className="py-3 px-3 text-right">Shares</th>
                  <th className="py-3 px-3 text-right">Avg Buy Price</th>
                  <th className="py-3 px-3 text-right">Current Price</th>
                  <th className="py-3 px-3 text-right">Invested Value</th>
                  <th className="py-3 px-3 text-right">Market Value</th>
                  <th className="py-3 px-3 text-right">Unrealized P&L</th>
                  <th className="py-3 px-3 text-center">Action</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-800/60">
                {activeHoldings.map((h) => {
                  const stock = stocks.find((s) => s.symbol === h.symbol);
                  const isProfit = h.unrealizedPnL >= 0;
                  return (
                    <tr key={h.symbol} className="hover:bg-slate-800/40 transition-colors">
                      <td className="py-3 px-3 font-bold text-slate-100">
                        <span className="px-2 py-1 rounded bg-blue-500/10 text-blue-400 border border-blue-500/20">
                          {h.symbol}
                        </span>
                      </td>
                      <td className="py-3 px-3 text-right text-slate-200">{h.quantity}</td>
                      <td className="py-3 px-3 text-right text-slate-300">₹{h.averageBuyPrice.toFixed(2)}</td>
                      <td className="py-3 px-3 text-right font-bold text-slate-100">₹{h.currentPrice.toFixed(2)}</td>
                      <td className="py-3 px-3 text-right text-slate-300">₹{h.investedValue.toFixed(2)}</td>
                      <td className="py-3 px-3 text-right font-bold text-slate-100">₹{h.marketValue.toFixed(2)}</td>
                      <td className={`py-3 px-3 text-right font-bold ${isProfit ? 'text-emerald-400' : 'text-rose-400'}`}>
                        {isProfit ? '+' : ''}₹{h.unrealizedPnL.toFixed(2)} ({isProfit ? '+' : ''}{h.pnlPercent.toFixed(2)}%)
                      </td>
                      <td className="py-3 px-3 text-center">
                        {stock && (
                          <button
                            onClick={() => onSelectStock(stock)}
                            className="px-2.5 py-1 rounded-lg bg-blue-600/15 text-blue-400 hover:bg-blue-600 hover:text-white transition-colors"
                          >
                            Trade
                          </button>
                        )}
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