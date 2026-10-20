import { memo } from 'react';
import { Handle, Position } from '@xyflow/react';

export const PiNode = memo(({ data }: { data: { label: string } }) => {
  return (
    <div className="group relative flex items-center justify-center px-4 py-2 rounded-xl bg-slate-900/90 border-2 border-cyan-500/60 shadow-[0_0_15px_rgba(6,182,212,0.15)] hover:border-cyan-400 hover:shadow-[0_0_20px_rgba(6,182,212,0.35)] transition-all min-w-[70px]">
      <div className="absolute -top-2 left-2 px-1.5 py-0.2 text-[9px] font-mono tracking-wider uppercase bg-cyan-950 text-cyan-400 border border-cyan-800 rounded">
        IN
      </div>
      <span className="font-mono font-bold text-sm text-cyan-200">{data.label}</span>
      <Handle
        type="source"
        position={Position.Bottom}
        className="!w-3 !h-3 !bg-cyan-400 !border-2 !border-slate-900 transition-transform group-hover:scale-125"
      />
    </div>
  );
});

export const AndNode = memo(({ data }: { data: { label: string; level?: number } }) => {
  return (
    <div className="group relative flex flex-col items-center justify-center px-4 py-2.5 rounded-2xl bg-slate-900/95 border-2 border-indigo-500/60 shadow-[0_0_18px_rgba(99,102,241,0.2)] hover:border-indigo-400 hover:shadow-[0_0_25px_rgba(99,102,241,0.4)] transition-all min-w-[85px]">
      <Handle
        type="target"
        position={Position.Top}
        className="!w-3 !h-3 !bg-indigo-400 !border-2 !border-slate-900 transition-transform group-hover:scale-125"
      />
      <div className="flex items-center gap-1.5">
        <span className="font-mono font-extrabold text-base text-indigo-200">&</span>
        {data.level !== undefined && (
          <span className="text-[10px] font-mono px-1.5 py-0.5 rounded bg-indigo-950/80 text-indigo-400 border border-indigo-800/60">
            L{data.level}
          </span>
        )}
      </div>
      {data.label && data.label !== '&' && (
        <span className="text-[10px] font-mono text-slate-400 mt-0.5">{data.label}</span>
      )}
      <Handle
        type="source"
        position={Position.Bottom}
        className="!w-3 !h-3 !bg-indigo-400 !border-2 !border-slate-900 transition-transform group-hover:scale-125"
      />
    </div>
  );
});

export const PoNode = memo(({ data }: { data: { label: string } }) => {
  return (
    <div className="group relative flex items-center justify-center px-4 py-2 rounded-xl bg-slate-900/90 border-2 border-amber-500/60 shadow-[0_0_15px_rgba(245,158,11,0.15)] hover:border-amber-400 hover:shadow-[0_0_20px_rgba(245,158,11,0.35)] transition-all min-w-[70px]">
      <Handle
        type="target"
        position={Position.Top}
        className="!w-3 !h-3 !bg-amber-400 !border-2 !border-slate-900 transition-transform group-hover:scale-125"
      />
      <div className="absolute -bottom-2 left-2 px-1.5 py-0.2 text-[9px] font-mono tracking-wider uppercase bg-amber-950 text-amber-400 border border-amber-800 rounded">
        OUT
      </div>
      <span className="font-mono font-bold text-sm text-amber-200">{data.label}</span>
    </div>
  );
});

export const ConstNode = memo(({ data }: { data: { label: string } }) => {
  return (
    <div className="group relative flex items-center justify-center px-3 py-1.5 rounded-lg bg-slate-900/90 border-2 border-slate-600/60 shadow-sm min-w-[50px]">
      <span className="font-mono font-bold text-xs text-slate-400">{data.label || 'CONST 0'}</span>
      <Handle
        type="source"
        position={Position.Bottom}
        className="!w-2.5 !h-2.5 !bg-slate-400 !border-2 !border-slate-900"
      />
    </div>
  );
});
