import React, { useState, useEffect, useRef } from 'react';

interface DownloadButtonProps {
  owner: string;
  repo: string;
}

type OS = 'Windows' | 'Linux' | 'Android' | 'Unknown';

export default function DownloadButton({ owner, repo }: DownloadButtonProps) {
  const [os, setOs] = useState<OS>('Unknown');
  const [mainDownloadUrl, setMainDownloadUrl] = useState(`https://github.com/${owner}/${repo}/releases`);
  const [version, setVersion] = useState<string>('');
  const [loading, setLoading] = useState<boolean>(true);
  const [isOpen, setIsOpen] = useState(false);
  
  const [releaseAssets, setReleaseAssets] = useState<any[]>([]);

  useEffect(() => {
    const userAgent = window.navigator.userAgent;
    let detectOS: OS = 'Unknown';

    if (userAgent.indexOf('Win') !== -1) detectOS = 'Windows';
    else if (userAgent.indexOf('Linux') !== -1) detectOS = 'Linux';
    else if (userAgent.indexOf('Android') !== -1) detectOS = 'Android';

    setOs(detectOS);

    fetch(`https://api.github.com/repos/${owner}/${repo}/releases/latest`)
      .then((res) => res.json())
      .then((data) => {
        if (data.tag_name) setVersion(data.tag_name);
        
        const assets = data.assets || [];
        setReleaseAssets(assets);

        let targetAsset;
        if (detectOS === 'Windows') {
          targetAsset = assets.find((a: any) => a.name.endsWith('.exe'));
        } else if (detectOS === 'Linux') {
          targetAsset = assets.find((a: any) => a.name.endsWith('.zip'));
        }

        if (targetAsset) {
          setMainDownloadUrl(targetAsset.browser_download_url);
        }
        setLoading(false);
      })
      .catch((err) => {
        console.error('Failed to fetch download releases', err);
        setLoading(false);
      });
  }, [owner, repo]);

  const buttonText = loading
    ? 'Checking latest version...'
    : version
    ? `Download ${version} for ${os !== 'Unknown' ? os : 'Desktop'}`
    : 'Download latest version';

  return (
    <div style={{ position: 'relative', display: 'inline-flex', alignItems: 'stretch' }}>
      {/* Main Download Action */}
      <a
        href={mainDownloadUrl}
        className="button button--secondary button--lg"
        style={{
          borderTopRightRadius: 0,
          borderBottomRightRadius: 0,
          borderRight: 'none', // Prevents double-thick borders in the middle
          display: 'flex',
          alignItems: 'center',
        }}
      >
        {buttonText}
      </a>

      {/* Dropdown Toggle Button */}
      <button
        className="button button--secondary button--lg"
        style={{
          borderTopLeftRadius: 0,
          borderBottomLeftRadius: 0,
          padding: '0 12px',
          borderLeft: '1px solid rgba(0, 0, 0, 0.1)', // A softer, more professional divider
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          cursor: 'pointer',
        }}
        onClick={() => setIsOpen(!isOpen)}
        aria-label="More download options"
      >
        {/* Using an SVG ensures perfect vertical centering unlike a text arrow */}
        <svg
          width="14"
          height="14"
          viewBox="0 0 24 24"
          fill="none"
          stroke="currentColor"
          strokeWidth="2.5"
          strokeLinecap="round"
          strokeLinejoin="round"
          style={{
            transform: isOpen ? 'rotate(180deg)' : 'none',
            transition: 'transform 0.2s ease',
          }}
        >
          <polyline points="6 9 12 15 18 9"></polyline>
        </svg>
      </button>

      {/* Dropdown Menu */}
      {isOpen && (
        <div
          style={{
            position: 'absolute',
            top: '100%',
            right: 0,
            marginTop: '8px',
            backgroundColor: 'var(--ifm-background-surface-color, #fff)',
            border: '1px solid var(--ifm-color-emphasis-300, #ccc)',
            borderRadius: 'var(--ifm-global-radius, 4px)', // Matches Docusaurus theme radius
            boxShadow: '0 4px 12px rgba(0,0,0,0.15)',
            minWidth: '220px',
            zIndex: 100,
            display: 'flex',
            flexDirection: 'column',
            overflow: 'hidden', // Keeps hover effects inside the rounded corners
          }}
        >
          {releaseAssets.map((asset) => (
            <a
              key={asset.id}
              href={asset.browser_download_url}
              style={{
                padding: '10px 16px',
                textDecoration: 'none',
                color: 'inherit',
                borderBottom: '1px solid var(--ifm-color-emphasis-200, #eee)',
                fontSize: '14px',
              }}
              // Quick inline hover effect simulation
              onMouseEnter={(e) => (e.currentTarget.style.backgroundColor = 'var(--ifm-color-emphasis-100, #f5f5f5)')}
              onMouseLeave={(e) => (e.currentTarget.style.backgroundColor = 'transparent')}
            >
              Download {asset.name}
            </a>
          ))}
          <a
            href={`https://github.com/${owner}/${repo}/releases/`}
            style={{
              padding: '10px 16px',
              textDecoration: 'none',
              fontWeight: 'bold',
              color: 'var(--ifm-color-primary)',
              fontSize: '14px',
              textAlign: 'center',
            }}
            onMouseEnter={(e) => (e.currentTarget.style.backgroundColor = 'var(--ifm-color-emphasis-100, #f5f5f5)')}
            onMouseLeave={(e) => (e.currentTarget.style.backgroundColor = 'transparent')}
          >
            View All Releases &rarr;
          </a>
        </div>
      )}
    </div>
  );
}