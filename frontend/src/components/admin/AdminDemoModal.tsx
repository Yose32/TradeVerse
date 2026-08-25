import React, { useState } from 'react';
import { Sliders, Zap, X, RefreshCw, CheckCircle2 } from 'lucide-react';
import { api } from '../../services/api';

interface AdminDemoModalProps {
  isOpen: boolean;
  onClose: () => void;
}

export const AdminDemoModal: React.FC<AdminDemoModalProps> = ({ isOpen, onClose }) => {
  const [regime, setRegime] = useState<string>('BULL');
  const [eventTitle, setEventTitle] = useState<string>('Semiconductor Supply Squeeze');
  const [epicentre, setEpicentre] = useState<string>('Semiconductors');
  const [impact, setImpact] = useState<number>(-0.10);
  const [statusMsg, setStatusMsg] = useState<string | null>(null);

  if (!isOpen) return null;

  const handleSetRegime = async (newRegime: string) => {
    try {
      await api.setMarketRegime(newRegime);
      setRegime(newRegime);
      setStatusMsg(`Market regime updated to ${newRegime}.`);
    } catch (e: any) {
      setStatusMsg(`Error: ${e.message}`);
    }
  };

  const handleTriggerEvent = async () => {
    try {
      const res = await api.triggerMarketEvent({
        title: eventTitle,
        description: `Macro shock triggered from ${epicentre}.`,
        epicentre,
        impact,
      });
      setStatusMsg(`Market Event Triggered: ${eventTitle} (Epicentre: ${res.epicentre}). Graph propagation active!`);
    } catch (e: any) {
      setStatusMsg(`Error: ${e.message}`);
    }
  };

  const handleManualTick = async () => {
    try {
      await api.triggerTick();
      setStatusMsg('Triggered instant simulation tick in C++ engine.');
    } catch (e: any) {
      setStatusMsg(`Error: ${e.message}`);
    }
  };

  return (
    <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-black/70 backdrop-blur-sm animate-fadeIn">
      <div className="max-w-lg w-full rounded-2xl bg-[#0c1326] border border-slate-700 shadow-2xl p-6 space-y-5">
        <div className="flex items-center justify-between pb-3 border-b border-slate-800">
          <div className="flex items-center gap-2.5">
            <div className="w-8 h-8 rounded-xl bg-blue-600/20 border border-blue-500/30 flex items-center justify-center text-blue-400">
              <Sliders className="w-5 h-5" />
            </div>
            <div>
              <h4 className="font-bold text-sm text-slate-100">Market Simulation Controller</h4>
              <p className="text-[11px] text-slate-400 font-mono">College Presentation / Admin Demo Mode</p>
            </div>
          </div>
          <button
            onClick={onClose}
            className="p-1.5 rounded-lg text-slate-400 hover:text-slate-200 hover:bg-slate-800 transition-colors"
          >
            <X className="w-5 h-5" />
          </button>
        </div>

        {statusMsg && (
          <div className="p-3 rounded-xl bg-blue-500/10 border border-blue-500/20 text-blue-300 text-xs flex items-center gap-2">
            <CheckCircle2 className="w-4 h-4 flex-shrink-0" />
            <span>{statusMsg}</span>
          </div>
        )}

        {/* Market Regime Selector */}
        <div className="space-y-2">
          <label className="text-xs font-mono text-slate-300 font-bold">
            Select Stochastic Market Regime:
          </label>
          <div className="grid grid-cols-3 gap-2">
            {[
              { id: 'BULL', label: 'Bull Market', color: 'hover:border-emerald-500' },
              { id: 'BEAR', label: 'Bear Market', color: 'hover:border-rose-500' },
              { id: 'SIDEWAYS', label: 'Sideways', color: 'hover:border-blue-500' },
              { id: 'HIGH_VOLATILITY', label: 'High Volatility', color: 'hover:border-gold' },
              { id: 'LOW_VOLATILITY', label: 'Low Volatility', color: 'hover:border-slate-400' },
            ].map((r) => (
              <button
                key={r.id}
                type="button"
                onClick={() => handleSetRegime(r.id)}
                className={`py-2 px-2.5 rounded-xl border text-xs font-mono font-semibold transition-all ${
                  regime === r.id
                    ? 'bg-blue-600 text-white border-blue-500 shadow-md'
                    : `bg-slate-900 border-slate-800 text-slate-300 ${r.color}`
                }`}
              >
                {r.label}
              </button>
            ))}
          </div>
        </div>

        {/* Trigger Macro Event */}
        <div className="space-y-3 pt-3 border-t border-slate-800">
          <label className="text-xs font-mono text-slate-300 font-bold">
            Trigger Macro Economic Shock (Graph Propagation):
          </label>

          <div className="space-y-2">
            <input
              type="text"
              value={eventTitle}
              onChange={(e) => setEventTitle(e.target.value)}
              placeholder="Event Title"
              className="w-full px-3 py-2 rounded-xl bg-slate-900 border border-slate-700 text-xs font-mono text-slate-100"
            />
            <div className="grid grid-cols-2 gap-2">
              <select
                value={epicentre}
                onChange={(e) => setEpicentre(e.target.value)}
                className="px-3 py-2 rounded-xl bg-slate-900 border border-slate-700 text-xs font-mono text-slate-100"
              >
                <option value="Semiconductors">Semiconductors</option>
                <option value="Technology">Technology</option>
                <option value="Consumer Electronics">Consumer Electronics</option>
                <option value="Cloud & Software">Cloud & Software</option>
                <option value="Automotive & CleanTech">Automotive</option>
                <option value="NVDA">NVDA Stock</option>
                <option value="AAPL">AAPL Stock</option>
              </select>

              <select
                value={impact}
                onChange={(e) => setImpact(parseFloat(e.target.value))}
                className="px-3 py-2 rounded-xl bg-slate-900 border border-slate-700 text-xs font-mono text-slate-100"
              >
                <option value={-0.12}>-12% Heavy Crash</option>
                <option value={-0.06}>-6% Moderate Dip</option>
                <option value={0.06}>+6% Positive Rally</option>
                <option value={0.15}>+15% Massive Breakthrough</option>
              </select>
            </div>
          </div>

          <button
            onClick={handleTriggerEvent}
            className="w-full py-2.5 rounded-xl bg-purple-600 hover:bg-purple-500 text-white text-xs font-bold font-mono transition-colors shadow-lg shadow-purple-500/20 flex items-center justify-center gap-2"
          >
            <Zap className="w-4 h-4" />
            <span>Propagate Shockwave Through Graph</span>
          </button>
        </div>

        <button
          onClick={handleManualTick}
          className="w-full py-2 rounded-xl bg-slate-800 hover:bg-slate-700 text-slate-300 text-xs font-mono font-semibold border border-slate-700 transition-colors flex items-center justify-center gap-2"
        >
          <RefreshCw className="w-3.5 h-3.5" />
          <span>Force 1 C++ Simulation Tick</span>
        </button>
      </div>
    </div>
  );
};