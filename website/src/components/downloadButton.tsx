import React, { useState, useEffect } from 'react';

interface DownloadButtonProps {
  owner: string;
  repo: string;
}

type OS = 'Windows' | 'macOS' | 'Linux' | 'Android' | 'Desktop';

export default function DownloadButton({ owner, repo }: DownloadButtonProps) {
  const [os, setOs] = useState<OS>('Desktop');
  const [version, setVersion] = useState<string>('');
  const [assets, setAssets] = useState<any[]>([]);
  const [primaryAssetUrl, setPrimaryAssetUrl] = useState<string>('');
  const [isOpen, setIsOpen] = useState(false);

  useEffect(() => {
    if (typeof window !== 'undefined') {
      const ua = window.navigator.userAgent;
      if (ua.includes('Win')) setOs('Windows');
      else if (ua.includes('Mac')) setOs('macOS');
      else if (ua.includes('Linux')) setOs('Linux');
      else if (ua.includes('Android')) setOs('Android');
    }
  }, []);

  useEffect(() => {
    let ignore = false
    const controller = new AbortController();
    fetch(`https://api.github.com/repos/${owner}/${repo}/releases?per_page=1`,{
      signal: controller.signal
    })
      .then((res) => res.json())
      .then((data) => {
        if(ignore) return;
        if (!data || data.length === 0) return;
        const release = data[0]; 
        setVersion(release.tag_name || release.name || '');
        
        const releaseAssets = release.assets || [];
        setAssets(releaseAssets);

        const searchOs = os === 'Desktop' ? 'Windows' : os; 
        let target = releaseAssets.find((a: any) => 
          a.name.toLowerCase().includes(searchOs.toLowerCase()) && a.name.endsWith('.zip')
        );
        
        if (!target) target = releaseAssets.find((a: any) => a.name.endsWith('.zip'));
        if (!target && releaseAssets.length > 0) target = releaseAssets[0];
        
        if (target) {
          setPrimaryAssetUrl(target.browser_download_url);
        }
      })
      .catch((error)=>{
        if(error.name==='AbortError') return;
        console.log(error)
      });
      
      return ()=>{
        ignore = true;
        controller.abort();
      };
  }, [owner, repo, os]);

  const btnText = version ? `Download ${version} for ${os}` : `Checking latest...`;

  return (
    <div className="button-group" style={{ position: 'relative', display: 'inline-flex' }}>
      {/* Main Download Button */}
      <a
        className="button button--primary button--lg"
        href={primaryAssetUrl || '#'}
        onClick={(e) => {
          if (!primaryAssetUrl) e.preventDefault();
        }}
        style={{ 
          pointerEvents: primaryAssetUrl ? 'auto' : 'none', 
          opacity: primaryAssetUrl ? 1 : 0.7 
        }}
      >
        {btnText}
      </a>

      {/* Dropdown Toggle */}
      <button
        className="button button--primary button--lg"
        onClick={() => setIsOpen(!isOpen)}
        style={{ padding: '0 12px' }}
        aria-label="More download options"
        aria-expanded={isOpen}
        aria-haspopup="true"
      >
        ▼
      </button>

      {/* Dropdown Menu */}
      {isOpen && (
        <div
          style={{
            position: 'absolute',
            top: '100%',
            right: 0,
            marginTop: '6px',
            backgroundColor: 'var(--ifm-background-surface-color, #ffffff)',
            border: '1px solid var(--ifm-color-emphasis-300, #cccccc)',
            borderRadius: 'var(--ifm-global-radius, 6px)',
            boxShadow: '0 4px 12px rgba(0,0,0,0.15)',
            minWidth: '240px',
            zIndex: 100,
            display: 'flex',
            flexDirection: 'column',
            overflow: 'hidden',
          }}
        >
          {assets.length === 0 && (
            <span style={{ padding: '12px', color: 'var(--ifm-font-color-base, #333)', fontSize: '14px', textAlign: 'center' }}>
              No files available
            </span>
          )}
          {assets.map((asset) => (
            <a
              key={asset.id}
              href={asset.browser_download_url}
              style={{
                padding: '12px 16px',
                textDecoration: 'none',
                color: 'var(--ifm-font-color-base, #333333)',
                borderBottom: '1px solid var(--ifm-color-emphasis-200, #eeeeee)',
                fontSize: '14px',
                transition: 'background-color 0.2s'
              }}
              onMouseEnter={(e) => (e.currentTarget.style.backgroundColor = 'var(--ifm-color-emphasis-100, #f5f5f5)')}
              onMouseLeave={(e) => (e.currentTarget.style.backgroundColor = 'transparent')}
            >
              {asset.name}
            </a>
          ))}
          <a
            href={`https://github.com/${owner}/${repo}/releases/`}
            target="_blank"
            rel="noopener noreferrer"
            style={{
              padding: '12px 16px',
              textDecoration: 'none',
              fontWeight: 'bold',
              color: 'var(--ifm-color-primary, #000)',
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