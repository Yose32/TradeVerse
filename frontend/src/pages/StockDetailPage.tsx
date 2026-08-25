import React, { useState } from 'react';
import { ArrowLeft, ExternalLink, Network, Layers, Activity } from 'lucide-react';
import { useTrading } from '../context/TradingContext';
import { CandlestickChart } from '../components/charts/CandlestickChart';
import { DepthChart } from '../components/charts/DepthChart';
import { OrderBookView } from '../components/trading/OrderBookView';
import { TradeForm } from '../components/trading/TradeForm';
import { Stock } from '../types';

interface StockDetailPageProps {
  stock: Stock;
  onBack: () => void;
}

export const StockDetailPage: React.FC<StockDetailPageProps> = ({ stock, onBack }) => {
  const { stocks, setSelectedStock } = useTrading();
  const [selectedPrice, setSelectedPrice] = useState<number | undefined>(undefined);

  const relatedStocks = stocks.filter((s) => s.sector === stock.sector && s.symbol !== stock.symbol);

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      {/* Back Button */}
      <button
        onClick={onBack}
        className="flex items-center gap-2 text-xs font-mono text-slate-400 hover:text-slate-100 transition-colors"
      >
        <ArrowLeft className="w-4 h-4" />
        <span>Back to Markets</span>
      </button>

      {/* Main 2-Col Layout */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Left Column: Charts */}
        <div className="lg:col-span-2 space-y-6">
          <CandlestickChart stock={stock} />
          <DepthChart symbol={stock.symbol} />

          {/* Related Sector Stocks */}
          {relatedStocks.length > 0 && (
            <div className="p-5 rounded-2xl glass-card border border-slate-800/80 space-y-3">
              <h4 className="font-bold text-sm text-slate-200 flex items-center gap-2">
                <Network className="w-4 h-4 text-purple-400" />
                <span>Correlated Sector Peers ({stock.sector})</span>
              </h4>
              <div className="grid grid-cols-1 sm:grid-cols-3 gap-3 font-mono text-xs">
                {relatedStocks.map((peer) => (
                  <div
                    key={peer.symbol}
                    onClick={() => setSelectedStock(peer)}
                    className="p-3 rounded-xl bg-slate-900 border border-slate-800 cursor-pointer hover:border-purple-500/40 transition-colors flex items-center justify-between"
                  >
                    <div>
                      <div className="font-bold text-slate-100">{peer.symbol}</div>
                      <div className="text-[11px] text-slate-400">₹{peer.currentPrice.toFixed(2)}</div>
                    </div>
                    <span className={peer.change >= 0 ? 'text-emerald-400 font-bold' : 'text-rose-400 font-bold'}>
                      {peer.change >= 0 ? '+' : ''}{peer.changePercent.toFixed(2)}%
                    </span>
                  </div>
                ))}
              </div>
            </div>
          )}
        </div>

        {/* Right Column: Order Entry & Depth Book */}
        <div className="space-y-6">
          <TradeForm stock={stock} prefillPrice={selectedPrice} />
          <OrderBookView symbol={stock.symbol} onPriceSelect={(p) => setSelectedPrice(p)} />
        </div>
      </div>
    </div>
  );
};