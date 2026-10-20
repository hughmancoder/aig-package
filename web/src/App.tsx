import { useState, useEffect, useCallback } from 'react';
import { Cpu, AlertCircle, Sun, Moon } from 'lucide-react';
import { useAigEngine } from './hooks/useAigEngine';
import type { AigProcessResult } from './lib/aigTypes';
import { AigGraphCanvas } from './components/AigGraphCanvas';
import { EquationInputBar } from './components/EquationInputBar';
import { PipelineControls, type ViewMode } from './components/PipelineControls';
import { MetricsPanel } from './components/MetricsPanel';

export function App() {
  const { simplifyEquation, isWasmLoaded, isLoading } = useAigEngine();

  const [theme, setTheme] = useState<'dark' | 'light'>('dark');
  const [equation, setEquation] = useState<string>('F = A & B & A & C');
  const [selectedPasses, setSelectedPasses] = useState<string[]>([
    'strash',
    'balance',
    'rewrite',
  ]);
  // Default to 'optimized' single tree view as requested by user
  const [viewMode, setViewMode] = useState<ViewMode>('optimized');
  const [result, setResult] = useState<AigProcessResult | null>(null);
  const [isProcessing, setIsProcessing] = useState<boolean>(false);

  const isDark = theme === 'dark';

  const handleSynthesize = useCallback(() => {
    if (!equation.trim()) return;
    setIsProcessing(true);
    setTimeout(() => {
      const res = simplifyEquation(equation, selectedPasses);
      setResult(res);
      setIsProcessing(false);
    }, 40);
  }, [equation, selectedPasses, simplifyEquation]);

  // Initial synthesis on mount
  useEffect(() => {
    handleSynthesize();
  }, [handleSynthesize]);

  const togglePass = (pass: string) => {
    setSelectedPasses((prev) =>
      prev.includes(pass) ? prev.filter((p) => p !== pass) : [...prev, pass]
    );
  };

  const setRecipe = (passes: string[]) => {
    setSelectedPasses(passes);
  };

  const toggleTheme = () => {
    setTheme((prev) => (prev === 'dark' ? 'light' : 'dark'));
  };

  return (
    <div
      className={`flex flex-col min-h-screen transition-colors duration-200 ${isDark ? 'bg-[#070b14] text-slate-100' : 'bg-slate-100 text-slate-900'
        }`}
    >
      {/* Top Navbar */}
      <header
        className={`sticky top-0 z-30 flex items-center justify-between px-6 py-3.5 border-b backdrop-blur-md transition-colors ${isDark ? 'bg-slate-950/85 border-slate-800/80' : 'bg-white/85 border-slate-200 shadow-sm'
          }`}
      >
        <div className="flex items-center gap-3">
          <div className="flex items-center justify-center w-9 h-9 rounded-xl bg-gradient-to-tr from-cyan-600 to-indigo-600 shadow-[0_0_15px_rgba(6,182,212,0.35)]">
            <Cpu className="w-5 h-5 text-white" />
          </div>
          <div>
            <div className="flex items-center gap-2">
              <span
                className={`font-extrabold tracking-tight text-base ${isDark
                    ? 'bg-gradient-to-r from-white via-slate-200 to-cyan-300 bg-clip-text text-transparent'
                    : 'text-slate-900'
                  }`}
              >
                AIG Studio
              </span>
              <span
                className={`px-2 py-0.5 rounded text-[10px] font-mono font-semibold border ${isDark
                    ? 'bg-cyan-950/80 text-cyan-400 border-cyan-800/60'
                    : 'bg-cyan-50 text-cyan-700 border-cyan-200'
                  }`}
              >
                v1.0 EDA
              </span>
            </div>
            <p className={`text-[11px] ${isDark ? 'text-slate-400' : 'text-slate-500'}`}>
              And-Inverter Graph Synthesis, Strashing & DAG Rewriting
            </p>
          </div>
        </div>

        {/* Engine Status & Theme Switcher */}
        <div className="flex items-center gap-4">
          {/* Engine status indicator */}
          <div
            className={`flex items-center gap-2 px-3 py-1.5 rounded-full border text-xs font-mono ${isDark ? 'bg-slate-900 border-slate-800 text-slate-300' : 'bg-slate-50 border-slate-200 text-slate-700'
              }`}
          >
            {isLoading ? (
              <>
                <div className="w-2 h-2 rounded-full bg-cyan-500 animate-ping" />
                <span>Initializing...</span>
              </>
            ) : (
              <>
                <div className="w-2 h-2 rounded-full bg-emerald-500 shadow-[0_0_8px_rgba(16,185,129,0.8)]" />
                <span className="font-medium text-emerald-600 dark:text-emerald-400">
                  {isWasmLoaded ? 'Wasm C++ Engine' : 'AIG Synthesis Engine'}
                </span>
              </>
            )}
          </div>

          {/* Quick Logic Legend */}
          <div
            className={`hidden md:flex items-center gap-3 text-[11px] font-mono ${isDark ? 'text-slate-400' : 'text-slate-500'
              }`}
          >
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-full bg-cyan-400" /> PI
            </span>
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-full bg-indigo-500" /> AND (&)
            </span>
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-full bg-amber-500" /> PO
            </span>
            <span className="flex items-center gap-1">
              <span className="w-2 h-2 rounded-full bg-rose-500" /> NOT bubble
            </span>
          </div>

          {/* Light / Dark Mode Toggle */}
          <button
            type="button"
            onClick={toggleTheme}
            className={`flex items-center justify-center w-8 h-8 rounded-lg border transition-all cursor-pointer ${isDark
                ? 'bg-slate-900 hover:bg-slate-800 border-slate-800 text-amber-400'
                : 'bg-white hover:bg-slate-100 border-slate-200 text-slate-700 shadow-sm'
              }`}
            title={isDark ? 'Switch to Light Theme' : 'Switch to Dark Theme'}
          >
            {isDark ? <Sun className="w-4 h-4" /> : <Moon className="w-4 h-4" />}
          </button>
        </div>
      </header>

      {/* Main Workspace */}
      <main className="flex-1 flex flex-col p-6 max-w-[1700px] w-full mx-auto gap-5">
        {/* Input Bar */}
        <EquationInputBar
          value={equation}
          onChange={setEquation}
          onSynthesize={handleSynthesize}
          isProcessing={isProcessing}
          theme={theme}
        />

        {/* Error Notification */}
        {result?.status === 'error' && (
          <div className="flex items-center gap-3 p-4 rounded-xl bg-rose-950/40 border border-rose-800 text-rose-300 text-sm font-mono shadow-lg">
            <AlertCircle className="w-5 h-5 shrink-0 text-rose-400" />
            <span>Synthesis Error: {result.error_message}</span>
          </div>
        )}

        {/* Pipeline Controls */}
        <PipelineControls
          selectedPasses={selectedPasses}
          onTogglePass={togglePass}
          onSetRecipe={setRecipe}
          viewMode={viewMode}
          onViewModeChange={setViewMode}
          theme={theme}
        />

        {/* Graph Canvases Section */}
        <div className="flex-1 min-h-[500px] w-full">
          {viewMode === 'split' ? (
            <div className="grid grid-cols-1 lg:grid-cols-2 gap-5 h-full">
              <div className="flex flex-col h-full">
                <AigGraphCanvas
                  data={result?.initial_graph || { nodes: [], edges: [] }}
                  title="Initial AIG Structure (Unoptimized)"
                  badge={`${result?.stats.initial_and_nodes ?? 0} AND Gates • L${result?.stats.initial_levels ?? 0}`}
                  badgeColor="slate"
                  theme={theme}
                />
              </div>
              <div className="flex flex-col h-full">
                <AigGraphCanvas
                  data={result?.graph || { nodes: [], edges: [] }}
                  title="Synthesized & Optimized AIG"
                  badge={`${result?.stats.final_and_nodes ?? 0} AND Gates • L${result?.stats.final_levels ?? 0}`}
                  badgeColor="emerald"
                  theme={theme}
                />
              </div>
            </div>
          ) : viewMode === 'initial' ? (
            <div className="h-full">
              <AigGraphCanvas
                data={result?.initial_graph || { nodes: [], edges: [] }}
                title="Initial AIG Structure (Unoptimized)"
                badge={`${result?.stats.initial_and_nodes ?? 0} AND Gates • L${result?.stats.initial_levels ?? 0}`}
                badgeColor="slate"
                theme={theme}
              />
            </div>
          ) : (
            /* Default: Single Optimized Tree */
            <div className="h-full">
              <AigGraphCanvas
                data={result?.graph || { nodes: [], edges: [] }}
                title="Synthesized & Optimized AIG"
                badge={`${result?.stats.final_and_nodes ?? 0} AND Gates • L${result?.stats.final_levels ?? 0}`}
                badgeColor="emerald"
                theme={theme}
              />
            </div>
          )}
        </div>

        {/* Bottom Metrics Panel */}
        {result && result.status === 'ok' && (
          <MetricsPanel
            stats={result.stats}
            originalExpr={result.original_expr}
            simplifiedExpr={result.simplified_expr}
            outputs={result.outputs}
            equivalent={result.equivalent}
            theme={theme}
          />
        )}
      </main>

      {/* Footer */}
      <footer
        className={`px-6 py-3.5 border-t text-center text-xs font-mono transition-colors ${isDark ? 'border-slate-900 bg-slate-950/70 text-slate-500' : 'border-slate-200 bg-white/80 text-slate-500'
          }`}
      >
        <span>AIG Synthesis & Optimisation Engine • Logic Synthesis, Strashing & DAG Rewriting</span>
      </footer>
    </div>
  );
}

export default App;
