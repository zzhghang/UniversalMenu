"""Build a browser-friendly manifest of extracted project assets and configured input brushes."""
import hashlib
import json
import pathlib
from extract_texture_png import ROOT, REF, texture

ASSETS = ROOT / 'WebPreview/assets'
texture_report = json.loads((REF / 'texture-manifest.json').read_text(encoding='utf-8'))
texture_by_name = {pathlib.Path(e['output']).stem: e for e in texture_report if 'output' in e}
source_by_name = {p.stem: p for p in (ROOT / 'Plugins/GameMainMenu/Content/Assets').rglob('*.uasset')}
configured = {}
for controller in ['CData_MouseKeyboard', 'CData_Gamepad']:
    package = json.loads((REF / (controller + '-properties.json')).read_text(encoding='utf-8'))
    properties = next(o['properties'] for o in package['objects'] if o['name'].startswith('Default__'))
    for entry in properties['InputBrushDataMap']:
        key = entry['Key']['KeyName']
        asset = entry['KeyBrush']['ResourceObject']
        if asset not in texture_by_name:
            result = texture(source_by_name[asset])
            texture_report.append(result)
            texture_by_name[asset] = result
        configured[key] = 'assets/' + asset + '.png'

supplemental = {
    'Escape': 'assets/T_Keyboard_Esc.png',
    'Up': 'assets/T_Keyboard_Arrow_Up.png',
    'Down': 'assets/T_Keyboard_Arrow_Down.png',
    'Left': 'assets/T_Keyboard_Arrow_Left.png',
    'Right': 'assets/T_Keyboard_Arrow_Right.png',
}
font_names = ['Regular', 'Normal', 'Light']
fonts = {name: {'file': 'assets/SourceHanSansCN-' + name + '.otf', 'bytes': (ASSETS / ('SourceHanSansCN-' + name + '.otf')).stat().st_size} for name in font_names}
font_styles = {}
for path in sorted(REF.glob('SText*-properties.json')):
    package = json.loads(path.read_text(encoding='utf-8'))
    properties = next(o['properties'] for o in package['objects'] if o['name'].startswith('Default__'))
    font = properties['Font']
    face = font['TypefaceFontName']
    if face == 'Default':
        face = 'Regular'
    font_styles[path.stem.removesuffix('-properties')] = {
        'face': face,
        'file': fonts[face]['file'],
        'storedPoints': font['Size'],
        'cssPixelsAtScale1': font['Size'] * 96 / 72,
        'linearColor': properties['Color'],
        'outline': font['OutlineSettings'],
        'skewAmount': font.get('SkewAmount', 0),
    }
movie_original = ROOT / 'Content/Movies/UELogoAnim.mp4'
movie_copy = ASSETS / 'UELogoAnim.mp4'
movie_sha = hashlib.sha256(movie_copy.read_bytes()).hexdigest()
assert movie_sha == hashlib.sha256(movie_original.read_bytes()).hexdigest()

manifest = {
    'pathBase': 'WebPreview/',
    'provenance': 'Fonts and textures extracted from project .uasset source data; startup movie copied byte-for-byte.',
    'fonts': fonts,
    'fontDefaultFace': 'Regular',
    'fontStyles': font_styles,
    'textures': {name: {'file': 'assets/' + name + '.png', 'width': item['width'], 'height': item['height'], 'source': item['source'], 'lossless': True} for name, item in sorted(texture_by_name.items())},
    'configuredKeyToFile': configured,
    'supplementalKeyToFile': supplemental,
    'keyToFile': {**configured, **supplemental},
    'alternateGamepadLabels': {name: 'assets/T_Xbox_' + name + '.png' for name in ['LT', 'RT', 'LB', 'RB']},
    'gamepadNote': 'Original CData_Gamepad maps trigger axes to XboxElite bottom paddles and shoulders to XboxElite top paddles. Alternate LT/RT/LB/RB files exist but are not the configured project brushes.',
    'videos': {'startup': {'file': 'assets/UELogoAnim.mp4', 'source': 'Content/Movies/UELogoAnim.mp4', 'bytes': movie_copy.stat().st_size, 'sha256': movie_sha}},
}
(REF / 'texture-manifest.json').write_text(json.dumps(texture_report, indent=2), encoding='utf-8')
(ROOT / 'WebPreview/assets-manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
for file in manifest['keyToFile'].values():
    assert (ROOT / 'WebPreview' / file).exists(), file
print('Textures:', len(texture_by_name), 'Fonts:', len(fonts), 'Configured key icons:', len(configured), 'Supplemental key icons:', len(supplemental))
print('Startup movie SHA-256 verified:', movie_sha)
