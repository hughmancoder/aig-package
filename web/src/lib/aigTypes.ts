export interface AigGraphNode {
  id: string;
  type: 'PI' | 'AND' | 'PO' | 'CONST';
  label: string;
  level: number;
}

export interface AigGraphEdge {
  id: string;
  source: string;
  target: string;
  inverted: boolean;
}

export interface AigGraphData {
  nodes: AigGraphNode[];
  edges: AigGraphEdge[];
}

export interface AigStats {
  initial_and_nodes: number;
  final_and_nodes: number;
  initial_levels: number;
  final_levels: number;
  num_inputs: number;
  num_outputs: number;
}

export interface OutputMapping {
  name: string;
  original: string;
  simplified: string;
}

export interface AigProcessResult {
  status: 'ok' | 'error';
  error_message: string;
  original_expr: string;
  simplified_expr: string;
  outputs: OutputMapping[];
  passes_applied: string[];
  equivalent: boolean | null;
  stats: AigStats;
  initial_graph: AigGraphData;
  graph: AigGraphData;
}
