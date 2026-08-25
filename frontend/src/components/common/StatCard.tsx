import React from 'react';
import { LucideIcon } from 'lucide-react';

interface StatCardProps {
  title: string;
  value: string;
  subtitle?: string;
  change?: number;
  changePercent?: number;
  icon: LucideIcon;
  variant?: 'default' | 'success' | 'danger' | 'brand';
}

export const StatCard: React.FC<StatCardProps> = ({
  title,
  value,
  subtitle,
  change,
  changePercent,
  icon: Icon,
  variant = 'default',
}) => {
  const getGlow = () => {
    if (variant === 'success' || (change !== undefined && change > 0)) return 'border-emerald-500/20 shadow-emerald-500/5';
    if (variant === 'danger' || (change !== undefined && change < 0)) return 'border-rose-500/20 shadow-rose-500/5';
    if (variant === 'brand') return 'border-blue-500/20 shadow-blue-500/5';
    return 'border-slate-800';
  };

  return (
    <div className={`p-5 rounded-2xl glass-card border shadow-lg transition-all hover:border-slate-700 ${getGlow()}`}>
      <div className="flex items-center justify-between mb-3">
        <span className="text-xs font-semibold uppercase tracking-wider text-slate-400">
          {title}
        </span>
        <div className="w-9 h-9 rounded-xl bg-slate-800/80 border border-slate-700/60 flex items-center justify-center text-slate-300">
          <Icon className="w-4 h-4" />
        </div>
      </div>

      <div className="space-y-1">
        <div className="text-2xl font-bold font-mono text-slate-100 tracking-tight">
          {value}
        </div>

        <div className="flex items-center gap-2 text-xs font-mono">
          {change !== undefined && (
            <span className={`font-semibold ${change >= 0 ? 'text-emerald-400' : 'text-rose-400'}`}>
              {change >= 0 ? '+' : ''}₹{Math.abs(change).toLocaleString('en-IN', { maximumFractionDigits: 2 })}
              {changePercent !== undefined && ` (${change >= 0 ? '+' : ''}${changePercent.toFixed(2)}%)`}
            </span>
          )}
          {subtitle && (
            <span className="text-slate-400">{subtitle}</span>
          )}
        </div>
      </div>
    </div>
  );
};