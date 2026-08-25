import React, { useState } from 'react';
import { ArrowRight, CheckCircle2, AlertCircle, Cpu, Zap, X } from 'lucide-react';
import { api } from '../../services/api';
import { useAuth } from '../../context/AuthContext';
import { useTrading } from '../../context/TradingContext';
import { Stock, OrderResponse } from '../../types';

interface TradeFormProps {
  stock: Stock;
  prefillPrice?: number;
}

export const TradeForm: React.FC<TradeFormProps> = ({ stock, prefillPrice }) => {
  const { isAuthenticated, user } = useAuth();
  const { portfolio, refreshPortfolio } = useTrading();

  const [side, setSide] = useState<'BUY' | 'SELL'>('BUY');
  const [orderType, setOrderType] = useState<'MARKET' | 'LIMIT'>('LIMIT');
  const [quantity, setQuantity] = useState<number>(10);
  const [price, setPrice] = useState<number>(prefillPrice || stock.currentPrice);
  const [isSubmitting, setIsSubmitting] = useState<boolean>(false);
  const [resultModal, setResultModal] = useState<OrderResponse | null>(null);
  const [errorMessage, setErrorMessage] = useState<string | null>(null);

  React.useEffect(() => {
    if (prefillPrice) {
      setPrice(prefillPrice);
    } else if (orderType === 'MARKET') {
      setPrice(stock.currentPrice);
    }
  }, [prefillPrice, stock.currentPrice, orderType]);

  const effectivePrice = orderType === 'MARKET' ? stock.currentPrice : price;
  const estimatedTotal = quantity * effectivePrice;

  const currentHolding = portfolio?.holdings.find((h) => h.symbol === stock.symbol);
  const availableShares = currentHolding ? currentHolding.quantity : 0;
  const availableCash = portfolio?.cashBalance || user?.cashBalance || 0;

  const handleQuickPercent = (pct: number) => {
    if (side === 'BUY') {
      const maxQty = Math.floor((availableCash * (pct / 100)) / (effectivePrice || 1));
      setQuantity(Math.max(1, maxQty));
    } else {
      const qty = Math.floor(availableShares * (pct / 100));
      setQuantity(Math.max(1, qty));
    }
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setErrorMessage(null);

    if (!isAuthenticated) {
      setErrorMessage('Please log in to execute trades.');
      return;
    }

    if (quantity <= 0) {
      setErrorMessage('Quantity must be greater than 0.');
      return;
    }

    if (side === 'BUY' && estimatedTotal > availableCash) {
      setErrorMessage(`Insufficient virtual cash. Needed: ₹${estimatedTotal.toFixed(2)}, Available: ₹${availableCash.toFixed(2)}`);
      return;
    }

    if (side === 'SELL' && quantity > availableShares) {
      setErrorMessage(`Insufficient shares. You own ${availableShares} shares of ${stock.symbol}.`);
      return;
    }

    setIsSubmitting(true);
    try {
      const res = await api.placeOrder({
        symbol: stock.symbol,
        type: orderType,
        side,
        quantity,
        price: effectivePrice,
      });

      setResultModal(res);
      await refreshPortfolio();
    } catch (err: any) {
      setErrorMessage(err.message || 'Order failed to process.');
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <>
      <div className="p-5 rounded-2xl glass-card border border-slate-800/80 shadow-xl space-y-4">
        {/* Buy / Sell Tabs */}
        <div className="grid grid-cols-2 p-1 rounded-xl bg-slate-900 border border-slate-800">
          <button
            type="button"
            onClick={() => setSide('BUY')}
            className={`py-2 rounded-lg font-bold text-xs uppercase tracking-wider transition-all ${
              side === 'BUY'
                ? 'bg-emerald-600 text-white shadow-md shadow-emerald-500/20'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Buy {stock.symbol}
          </button>
          <button
            type="button"
            onClick={() => setSide('SELL')}
            className={`py-2 rounded-lg font-bold text-xs uppercase tracking-wider transition-all ${
              side === 'SELL'
                ? 'bg-rose-600 text-white shadow-md shadow-rose-500/20'
                : 'text-slate-400 hover:text-slate-200'
            }`}
          >
            Sell {stock.symbol}
          </button>
        </div>

        {/* Order Type Toggle (Market vs Limit) */}
        <div className="flex items-center justify-between text-xs font-mono">
          <span className="text-slate-400">Order Type:</span>
          <div className="flex gap-1.5">
            <button
              type="button"
              onClick={() => setOrderType('LIMIT')}
              className={`px-3 py-1 rounded-lg border text-xs font-semibold transition-all ${
                orderType === 'LIMIT'
                  ? 'bg-blue-600/20 border-blue-500 text-blue-400'
                  : 'border-slate-800 text-slate-400 hover:border-slate-700'
              }`}
            >
              Limit
            </button>
            <button
              type="button"
              onClick={() => setOrderType('MARKET')}
              className={`px-3 py-1 rounded-lg border text-xs font-semibold transition-all ${
                orderType === 'MARKET'
                  ? 'bg-blue-600/20 border-blue-500 text-blue-400'
                  : 'border-slate-800 text-slate-400 hover:border-slate-700'
              }`}
            >
              Market
            </button>
          </div>
        </div>

        <form onSubmit={handleSubmit} className="space-y-4">
          {/* Limit Price Input */}
          {orderType === 'LIMIT' && (
            <div className="space-y-1.5">
              <label className="text-xs font-mono text-slate-400 flex justify-between">
                <span>Limit Price</span>
                <span className="text-slate-300">Market: ₹{stock.currentPrice.toFixed(2)}</span>
              </label>
              <div className="relative">
                <span className="absolute left-3 top-2.5 font-mono text-sm text-slate-400">₹</span>
                <input
                  type="number"
                  step="0.05"
                  min="0.05"
                  value={price}
                  onChange={(e) => setPrice(parseFloat(e.target.value) || 0)}
                  className="w-full pl-8 pr-3 py-2 rounded-xl bg-slate-900 border border-slate-700/80 text-sm font-mono text-slate-100 focus:outline-none focus:border-blue-500 transition-colors"
                />
              </div>
            </div>
          )}

          {/* Quantity Input */}
          <div className="space-y-1.5">
            <label className="text-xs font-mono text-slate-400 flex justify-between">
              <span>Quantity (Shares)</span>
              <span>
                {side === 'BUY'
                  ? `Max Afford: ${Math.floor(availableCash / (effectivePrice || 1))}`
                  : `Owned: ${availableShares}`}
              </span>
            </label>
            <input
              type="number"
              min="1"
              step="1"
              value={quantity}
              onChange={(e) => setQuantity(parseInt(e.target.value, 10) || 0)}
              className="w-full px-3 py-2 rounded-xl bg-slate-900 border border-slate-700/80 text-sm font-mono text-slate-100 focus:outline-none focus:border-blue-500 transition-colors"
            />
          </div>

          {/* Quick Percent Buttons */}
          <div className="grid grid-cols-4 gap-1.5">
            {[25, 50, 75, 100].map((pct) => (
              <button
                key={pct}
                type="button"
                onClick={() => handleQuickPercent(pct)}
                className="py-1 rounded-lg bg-slate-800/80 border border-slate-700 text-[11px] font-mono font-semibold text-slate-300 hover:bg-slate-700 transition-colors"
              >
                {pct}%
              </button>
            ))}
          </div>

          {/* Order Summary Box */}
          <div className="p-3 rounded-xl bg-slate-900/90 border border-slate-800 space-y-1.5 text-xs font-mono">
            <div className="flex justify-between text-slate-400">
              <span>Estimated Value:</span>
              <span className="text-slate-200 font-semibold">₹{estimatedTotal.toLocaleString('en-IN', { maximumFractionDigits: 2 })}</span>
            </div>
            <div className="flex justify-between text-slate-400">
              <span>Trading Fees (Paper):</span>
              <span className="text-emerald-400">₹0.00</span>
            </div>
            <div className="flex justify-between text-slate-400 pt-1 border-t border-slate-800">
              <span>Available Balance:</span>
              <span className="text-slate-100 font-bold">
                {side === 'BUY' ? `₹${availableCash.toLocaleString('en-IN', { maximumFractionDigits: 2 })}` : `${availableShares} Shares`}
              </span>
            </div>
          </div>

          {/* Error Message Banner */}
          {errorMessage && (
            <div className="p-3 rounded-xl bg-rose-500/10 border border-rose-500/20 text-rose-300 text-xs flex items-center gap-2">
              <AlertCircle className="w-4 h-4 flex-shrink-0" />
              <span>{errorMessage}</span>
            </div>
          )}

          {/* Submit Button */}
          <button
            type="submit"
            disabled={isSubmitting}
            className={`w-full py-3 rounded-xl font-bold text-sm tracking-wide flex items-center justify-center gap-2 transition-all shadow-lg ${
              side === 'BUY'
                ? 'bg-emerald-600 hover:bg-emerald-500 text-white shadow-emerald-500/25'
                : 'bg-rose-600 hover:bg-rose-500 text-white shadow-rose-500/25'
            }`}
          >
            {isSubmitting ? (
              <span className="flex items-center gap-2">
                <Cpu className="w-4 h-4 animate-spin" />
                <span>Routing to C++ Engine...</span>
              </span>
            ) : (
              <>
                <span>Submit {side} Order</span>
                <ArrowRight className="w-4 h-4" />
              </>
            )}
          </button>
        </form>
      </div>

      {/* C++ DSA Match Result Modal */}
      {resultModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/70 backdrop-blur-sm animate-fadeIn">
          <div className="max-w-lg w-full rounded-2xl bg-[#0d1527] border border-slate-700 shadow-2xl p-6 space-y-4">
            <div className="flex items-center justify-between pb-3 border-b border-slate-800">
              <div className="flex items-center gap-2.5">
                <div className="w-8 h-8 rounded-lg bg-emerald-500/20 border border-emerald-500/30 flex items-center justify-center text-emerald-400">
                  <CheckCircle2 className="w-5 h-5" />
                </div>
                <div>
                  <h4 className="font-bold text-base text-slate-100">Order Executed</h4>
                  <p className="text-xs text-slate-400 font-mono">ID: {resultModal.orderId}</p>
                </div>
              </div>
              <button
                onClick={() => setResultModal(null)}
                className="p-1 rounded-lg text-slate-400 hover:text-slate-200 hover:bg-slate-800"
              >
                <X className="w-5 h-5" />
              </button>
            </div>

            <div className="grid grid-cols-3 gap-2 text-center text-xs font-mono">
              <div className="p-2.5 rounded-xl bg-slate-900/80 border border-slate-800">
                <div className="text-slate-400 text-[10px]">Status</div>
                <div className="font-bold text-emerald-400">{resultModal.status}</div>
              </div>
              <div className="p-2.5 rounded-xl bg-slate-900/80 border border-slate-800">
                <div className="text-slate-400 text-[10px]">Filled Qty</div>
                <div className="font-bold text-slate-100">{resultModal.filledQuantity}</div>
              </div>
              <div className="p-2.5 rounded-xl bg-slate-900/80 border border-slate-800">
                <div className="text-slate-400 text-[10px]">Trades Matched</div>
                <div className="font-bold text-blue-400">{resultModal.tradesExecuted}</div>
              </div>
            </div>

            {/* C++ DSA Trace Log */}
            <div className="space-y-2">
              <div className="flex items-center gap-2 text-xs font-mono font-bold text-purple-400">
                <Cpu className="w-4 h-4" />
                <span>C++ DSA Engine Execution Trace</span>
              </div>
              <div className="max-h-48 overflow-y-auto p-3 rounded-xl bg-black/50 border border-slate-800 font-mono text-[11px] text-slate-300 space-y-1.5">
                {resultModal.dsaTrace.map((step, idx) => (
                  <div key={idx} className="flex gap-2">
                    <span className="text-slate-500 select-none">[{idx + 1}]</span>
                    <span className="text-slate-200">{step}</span>
                  </div>
                ))}
              </div>
            </div>

            <button
              onClick={() => setResultModal(null)}
              className="w-full py-2.5 rounded-xl bg-blue-600 hover:bg-blue-500 text-white text-xs font-bold transition-colors"
            >
              Done & View Updated Portfolio
            </button>
          </div>
        </div>
      )}
    </>
  );
};