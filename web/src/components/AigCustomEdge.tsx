import { memo } from 'react';
import { BaseEdge, getBezierPath, EdgeLabelRenderer } from '@xyflow/react';
import type { EdgeProps } from '@xyflow/react';

export const AigCustomEdge = memo(({
  id,
  sourceX,
  sourceY,
  targetX,
  targetY,
  sourcePosition,
  targetPosition,
  style = {},
  markerEnd,
  data,
}: EdgeProps<any>) => {
  const [edgePath, labelX, labelY] = getBezierPath({
    sourceX,
    sourceY,
    sourcePosition,
    targetX,
    targetY,
    targetPosition,
  });

  const isInverted = Boolean(data?.inverted);

  const edgeStyle = isInverted
    ? {
        ...style,
        stroke: '#f43f5e', // rose-500
        strokeWidth: 2,
        strokeDasharray: '6, 4',
      }
    : {
        ...style,
        stroke: '#64748b', // slate-500
        strokeWidth: 2,
      };

  return (
    <>
      <BaseEdge
        id={id}
        path={edgePath}
        style={edgeStyle}
        markerEnd={markerEnd}
        className={isInverted ? 'inverted-edge' : ''}
      />
      {isInverted && (
        <EdgeLabelRenderer>
          <div
            style={{
              position: 'absolute',
              transform: `translate(-50%, -50%) translate(${labelX}px,${labelY}px)`,
              pointerEvents: 'all',
            }}
            className="flex items-center justify-center w-5 h-5 rounded-full bg-slate-950 border-2 border-rose-500 shadow-[0_0_10px_rgba(244,63,94,0.5)] z-10"
            title="Inverted / Complemented Edge (NOT)"
          >
            <div className="w-1.5 h-1.5 rounded-full bg-rose-400 animate-pulse" />
          </div>
        </EdgeLabelRenderer>
      )}
    </>
  );
});
