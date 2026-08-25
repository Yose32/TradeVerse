import React, { useState, useEffect } from 'react';
import { Clock, CheckCircle2, XCircle, AlertCircle, RefreshCw } from 'lucide-react';
import { api } from '../services/api';
import { Order } from '../types';

export const OrdersPage: React.FC = () => {
  const [orders, setOrders] = useState<Order[]>([]);
  const [filter, setFilter] = useState<'ALL' | 'OPEN' | 'FILLED' | 'CANCELLED'>('ALL');
  const [loading, setLoading] = useState<boolean>(true);

  const fetchOrders = async () => {
    try {
      const data = await api.getOrders();
      setOrders(data);
    } catch (e) {
      console.error(e);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchOrders();
    const interval = setInterval(fetchOrders, 2000);
    return () => clearInterval(interval);
  }, []);

  const filteredOrders = orders.filter((o) => {
    if (filter === 'OPEN') return o.status === 'PENDING' || o.status === 'PARTIALLY_FILLED';
    if (filter === 'FILLED') return o.status === 'FILLED';
    if (filter === 'CANCELLED') return o.status === 'CANCELLED';
    return true;
  });

  return (
    <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6 space-y-6 animate-fadeIn">
      <div className="flex flex-wrap items-center justify-between gap-4">
        <div>
          <h1 className="text-2xl font-extrabold text-slate-100">Order Management</h1>
          <p className="text-xs text-slate-400 mt-0.5">
            Track limit and market orders processed by the C++ Matching Engine
          </p>
        </div>

        {/* Filter Tabs */}
        <div className="flex gap-1.5 p-1 rounded-xl bg-slate-900 border border-slate-800 font-mono text-xs">
          {(['ALL', 'OPEN', 'FILLED', 'CANCELLED'] as const).map((f) => (
            <button
              key={f}
              onClick={() => setFilter(f)}
              className={`px-3 py-1.5 rounded-lg font-semibold transition-all ${
                filter === f
                  ? 'bg-blue-600 text-white shadow-md'
                  : 'text-slate-400 hover:text-slate-200'
              }`}
            >
              {f}
            </button>
          ))}
        </div>
      </div>

      <div className="rounded-2xl glass-card border border-slate-800/80 shadow-2xl overflow-hidden p-5 space-y-3">
        {filteredOrders.length === 0 ? (
          <div className="py-12 text-center text-slate-500 font-mono text-xs">
            {loading ? 'Loading orders from database...' : 'No orders found matching filter.'}
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left font-mono text-xs">
              <thead>
                <tr className="border-b border-slate-800 bg-slate-900/60 text-slate-400 uppercase text-[10px] tracking-wider">
                  <th className="py-3 px-3">Order ID</th>
                  <th className="py-3 px-3">Symbol</th>
                  <th className="py-3 px-3">Type</th>
                  <th className="py-3 px-3">Side</th>
                  <th className="py-3 px-3 text-right">Quantity</th>
                  <th className="py-3 px-3 text-right">Filled</th>
                  <th className="py-3 px-3 text-right">Price</th>
                  <th className="py-3 px-3 text-center">Status</th>
                  <th className="py-3 px-3 text-right">Time</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-slate-800/60">
                {filteredOrders.map((o) => {
                  const isBuy = o.side === 'BUY';
                  return (
                    <tr key={o.orderId} className="hover:bg-slate-800/40 transition-colors">
                      <td className="py-3 px-3 font-bold text-slate-300">{o.orderId}</td>
                      <td className="py-3 px-3 font-bold text-slate-100">{o.symbol}</td>
                      <td className="py-3 px-3 text-slate-400">{o.type}</td>
                      <td className="py-3 px-3 font-bold">
                        <span className={`px-2 py-0.5 rounded text-[11px] ${
                          isBuy ? 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/20' : 'bg-rose-500/10 text-rose-400 border border-rose-500/20'
                        }`}>
                          {o.side}
                        </span>
                      </td>
                      <td className="py-3 px-3 text-right text-slate-200">{o.quantity}</td>
                      <td className="py-3 px-3 text-right text-slate-300 font-bold">{o.filledQuantity}</td>
                      <td className="py-3 px-3 text-right font-bold text-slate-100">₹{o.price.toFixed(2)}</td>
                      <td className="py-3 px-3 text-center">
                        <span className={`px-2 py-0.5 rounded-full text-[10px] font-bold ${
                          o.status === 'FILLED'
                            ? 'bg-emerald-500/10 text-emerald-400 border border-emerald-500/30'
                            : o.status === 'PARTIALLY_FILLED'
                            ? 'bg-blue-500/10 text-blue-400 border border-blue-500/30'
                            : o.status === 'CANCELLED'
                            ? 'bg-rose-500/10 text-rose-400 border border-rose-500/30'
                            : 'bg-amber-500/10 text-amber-400 border border-amber-500/30'
                        }`}>
                          {o.status}
                        </span>
                      </td>
                      <td className="py-3 px-3 text-right text-slate-400 text-[11px]">
                        {new Date(o.timestamp).toLocaleTimeString()}
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