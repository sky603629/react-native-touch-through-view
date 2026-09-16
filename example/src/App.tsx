import React, {
  memo,
  useCallback,
  useEffect,
  useMemo,
  useRef,
  useState,
} from 'react';
import {
  Button,
  Image,
  ScrollView,
  StyleSheet,
  Text,
  View,
} from 'react-native';
import ImageZoomBase from '@react-native-ohos/react-native-image-pan-zoom';
import {
  TouchThroughView,
  TouchThroughWrapper,
} from 'react-native-touch-through-view';

const ImageZoom = ImageZoomBase as unknown as React.ComponentType<
  React.ComponentProps<typeof ImageZoomBase> & { children?: React.ReactNode }
>;

type ZoomStats = {
  moves: number;
  clicks: number;
  status: string;
  scale: number;
  x: number;
  y: number;
};

const ZoomStatsPanel = memo(function ZoomStatsPanel({
  statsRef,
}: {
  statsRef: React.RefObject<ZoomStats>;
}) {
  const [stats, setStats] = useState(statsRef.current);

  useEffect(() => {
    // Refresh diagnostics independently of the gesture and image rendering.
    const timer = setInterval(() => {
      setStats(current =>
        current === statsRef.current ? current : statsRef.current,
      );
    }, 100);
    return () => clearInterval(timer);
  }, [statsRef]);

  return (
    <View style={styles.statsCard}>
      <Text style={styles.zoomStats}>
        status={stats.status} moves={stats.moves} clicks={stats.clicks}
      </Text>
      <Text style={styles.zoomStats}>
        scale={stats.scale.toFixed(2)} x={Math.round(stats.x)} y=
        {Math.round(stats.y)}
      </Text>
    </View>
  );
});

const TouchThroughOverlay = memo(function TouchThroughOverlay() {
  const rows = useMemo(
    () => Array.from({ length: 30 }, (_, index) => `Item ${index}`),
    [],
  );

  return (
    <TouchThroughWrapper style={styles.scrollWrapper}>
      <ScrollView style={styles.scroller}>
        <TouchThroughView style={styles.touchThroughView} />
        {rows.map(row => (
          <Text key={row} style={styles.itemRow}>
            {row}
          </Text>
        ))}
      </ScrollView>
    </TouchThroughWrapper>
  );
});

export function ReactNativeTouchThroughViewExample(): React.JSX.Element {
  const [playCount, setPlayCount] = useState(0);
  const statsRef = useRef<ZoomStats>({
    moves: 0,
    clicks: 0,
    status: 'idle',
    scale: 1,
    x: 0,
    y: 0,
  });

  const handleZoomClick = useCallback(() => {
    statsRef.current = {
      ...statsRef.current,
      clicks: statsRef.current.clicks + 1,
      status: 'clicked',
    };
  }, []);

  const handleZoomMove = useCallback<
    NonNullable<React.ComponentProps<typeof ImageZoomBase>['onMove']>
  >(position => {
    statsRef.current = {
      ...statsRef.current,
      moves: statsRef.current.moves + 1,
      status: position.type || 'moving',
      scale: position.scale,
      x: position.positionX,
      y: position.positionY,
    };
  }, []);

  const handlePlayPress = () => {
    const nextCount = playCount + 1;
    console.info(`[RNTTV] Play pressed, nextCount=${nextCount}`);
    setPlayCount(nextCount);
  };

  return (
    <View style={styles.container}>
      <View style={styles.artistInfo}>
        <View style={styles.zoomSurface}>
          <Text style={styles.zoomTitle}>Pan / Zoom Through Test</Text>
          <Text style={styles.zoomSubtitle}>
            Drag in all directions, pinch to zoom, and tap to verify lower-layer
            gestures.
          </Text>
          <View style={styles.zoomStage}>
            <ImageZoom
              cropWidth={320}
              cropHeight={180}
              imageWidth={420}
              imageHeight={220}
              panToMove
              pinchToZoom
              enableDoubleClickZoom
              minScale={1}
              maxScale={3}
              onClick={handleZoomClick}
              onMove={handleZoomMove}
              style={styles.zoomCrop}>
              <View style={styles.zoomContent}>
                <Image
                  source={require('./album.jpg')}
                  style={styles.zoomImage}
                />
                <View style={styles.zoomBadge}>
                  <Text style={styles.zoomBadgeText}>
                    Drag me through the header
                  </Text>
                </View>
              </View>
            </ImageZoom>
          </View>
        </View>

        <ZoomStatsPanel statsRef={statsRef} />
        <Button title={`Play (${playCount})`} onPress={handlePlayPress} />
      </View>

      <TouchThroughOverlay />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#eee',
    paddingTop: 20,
  },
  artistInfo: {
    position: 'absolute',
    top: 20,
    left: 0,
    right: 0,
    height: 460,
    zIndex: 0,
    alignItems: 'center',
  },
  scroller: {
    zIndex: 1,
  },
  scrollWrapper: {
    flex: 1,
  },
  touchThroughView: {
    height: 460,
    flex: 1,
  },
  zoomSurface: {
    width: 344,
    borderRadius: 18,
    borderWidth: 2,
    borderStyle: 'dashed',
    borderColor: '#0b6bcb',
    backgroundColor: 'rgba(255,255,255,0.92)',
    padding: 12,
  },
  zoomStage: {
    marginTop: 10,
    width: 320,
    height: 180,
    alignSelf: 'center',
    borderRadius: 16,
    overflow: 'hidden',
    backgroundColor: '#d9e9f9',
  },
  zoomTitle: {
    fontSize: 16,
    fontWeight: '700',
    color: '#0b2f4f',
  },
  zoomSubtitle: {
    marginTop: 4,
    fontSize: 12,
    color: '#36556f',
  },
  zoomStats: {
    fontSize: 12,
    color: '#10385b',
  },
  statsCard: {
    width: 344,
    marginTop: 12,
    marginBottom: 12,
    paddingHorizontal: 14,
    paddingVertical: 10,
    borderRadius: 16,
    backgroundColor: 'rgba(255,255,255,0.94)',
    borderWidth: 1,
    borderColor: '#d3e2f1',
    gap: 4,
  },
  zoomCrop: {
    width: 320,
    height: 180,
    backgroundColor: '#d9e9f9',
  },
  zoomContent: {
    width: 420,
    height: 220,
    backgroundColor: '#d6e7f7',
  },
  zoomImage: {
    width: 420,
    height: 220,
    opacity: 0.88,
  },
  zoomBadge: {
    position: 'absolute',
    left: 20,
    top: 18,
    paddingHorizontal: 12,
    paddingVertical: 8,
    borderRadius: 999,
    backgroundColor: 'rgba(11,47,79,0.82)',
  },
  zoomBadgeText: {
    color: '#ffffff',
    fontSize: 13,
    fontWeight: '700',
  },
  itemRow: {
    backgroundColor: '#ddd',
    padding: 20,
    borderBottomWidth: 5,
    borderBottomColor: '#000',
  },
});

export default {
  displayName: 'react-native-touch-through-view',
  framework: 'React',
  category: 'UI',
  title: 'react-native-touch-through-view',
  documentationURL: '',
  description:
    'Source-library style demo for touch-through over a scroll view.',
  examples: [
    {
      title: 'source example',
      render: function (): React.JSX.Element {
        return <ReactNativeTouchThroughViewExample />;
      },
    },
  ],
};
