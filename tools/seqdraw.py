"""Tiny sequence-diagram builder that writes `.drawio.svg` files.

Each output file is a plain SVG (renders on GitHub) that also embeds the
draw.io model, so it can be opened and edited directly in draw.io / diagrams.net.
"""
from xml.sax.saxutils import escape, quoteattr

FONT_MONO = "Consolas, 'DejaVu Sans Mono', monospace"
FONT_SANS = "Helvetica, Arial, sans-serif"
CH_MONO = 6.7   # px per char, 11px monospace
CH_SANS = 6.4   # px per char, 11px sans
LINE_H = 15

# participant kinds -> (fill, stroke)
KIND_COLORS = {
    "actor":  ("#ffe6cc", "#d79b00"),   # player / buttons
    "kernel": ("#e1d5e7", "#9673a6"),   # AK timer / scheduler
    "screen": ("#dae8fc", "#6c8ebf"),   # display / screens
    "object": ("#d5e8d4", "#82b366"),   # game object tasks
    "store":  ("#f5f5f5", "#666666"),   # EEPROM, buzzer, LCD
}

TOP = 20
HEAD_H = 46
MARGIN_X = 30
MIN_GAP = 150


class Seq:
    def __init__(self, title):
        self.title = title
        self.parts = []          # (key, label, kind)
        self.items = []          # timeline items

    # ---- building API -------------------------------------------------
    def part(self, key, label, kind="object"):
        self.parts.append((key, label, kind))
        return self

    def msg(self, src, dst, text, style="sync"):
        """style: sync (solid, filled arrow), async (dashed, open arrow: timer / posted later)."""
        if src == dst:
            return self.self_(src, text, style)
        self.items.append(("msg", src, dst, text, style))
        return self

    def self_(self, who, text, style="sync"):
        self.items.append(("self", who, text, style))
        return self

    def note(self, a, b, text):
        self.items.append(("note", a, b, text))
        return self

    def frag(self, kind, guard):
        self.items.append(("frag", kind, guard))
        return self

    def else_(self, guard):
        self.items.append(("else", guard))
        return self

    def end(self):
        self.items.append(("end",))
        return self

    def gap(self, h=10):
        self.items.append(("gap", h))
        return self

    # ---- layout --------------------------------------------------------
    def _layout(self):
        idx = {k: i for i, (k, _, _) in enumerate(self.parts)}
        n = len(self.parts)
        head_w = [max(110, max(len(l) for l in label.split("\n")) * 7.6 + 24) for _, label, _ in self.parts]
        gaps = [max(MIN_GAP, (head_w[i] + head_w[i + 1]) / 2 + 20) for i in range(n - 1)]

        def need(i, j, width):
            lo, hi = min(i, j), max(i, j)
            if lo == hi:
                return
            have = sum(gaps[lo:hi])
            if have < width:
                extra = (width - have) / (hi - lo)
                for g in range(lo, hi):
                    gaps[g] += extra

        for it in self.items:
            if it[0] == "msg":
                width = max(len(l) for l in it[3].split("\n")) * CH_MONO + 30
                need(idx[it[1]], idx[it[2]], width)
            elif it[0] == "self":
                width = max(len(l) for l in it[2].split("\n")) * CH_MONO + 50
                i = idx[it[1]]
                if i < n - 1:
                    need(i, i + 1, width)
            elif it[0] == "note":
                i, j = idx[it[1]], idx[it[2]]
                width = max(len(l) for l in it[3].split("\n")) * CH_SANS + 30
                if i == j:
                    if i < n - 1:
                        need(i, i + 1, width / 2 + 40)
                else:
                    need(i, j, width - 60)

        xs = [MARGIN_X + head_w[0] / 2 + 40]
        for g in gaps:
            xs.append(xs[-1] + g)
        width = xs[-1] + head_w[-1] / 2 + MARGIN_X + 40
        return idx, xs, head_w, width

    # ---- render --------------------------------------------------------
    def save(self, path):
        idx, xs, head_w, width = self._layout()
        svg = []      # svg body elements
        cells = []    # draw.io cells
        cid = [2]

        def new_id():
            cid[0] += 1
            return f"c{cid[0]}"

        def html(text):
            return escape(text).replace("\n", "<br>")

        y = TOP + HEAD_H + 24
        frag_stack = []
        frag_left = MARGIN_X
        frag_right = width - MARGIN_X

        def text_svg(x, y0, text, anchor="middle", mono=True, size=11, weight="normal", color="#000"):
            fam = FONT_MONO if mono else FONT_SANS
            out = []
            for k, line in enumerate(text.split("\n")):
                out.append(
                    f'<text x="{x:.1f}" y="{y0 + k * LINE_H:.1f}" font-family="{fam}" font-size="{size}" '
                    f'font-weight="{weight}" fill="{color}" text-anchor="{anchor}">{escape(line)}</text>'
                )
            return "".join(out)

        for it in self.items:
            kind = it[0]
            if kind == "msg":
                _, a, b, text, style = it
                lines = text.split("\n")
                y += (len(lines) - 1) * LINE_H
                x1, x2 = xs[idx[a]], xs[idx[b]]
                dashed = style == "async"
                marker = "url(#open)" if dashed else "url(#filled)"
                dash = ' stroke-dasharray="6,4"' if dashed else ""
                svg.append(f'<line x1="{x1}" y1="{y}" x2="{x2}" y2="{y}" stroke="#222" stroke-width="1.3"{dash} marker-end="{marker}"/>')
                tw = max(len(l) for l in lines) * CH_MONO + 8
                ty = y - 6 - (len(lines) - 1) * LINE_H
                svg.append(f'<rect x="{(x1 + x2) / 2 - tw / 2:.1f}" y="{ty - 11:.1f}" width="{tw:.1f}" '
                           f'height="{len(lines) * LINE_H:.1f}" fill="#ffffff" fill-opacity="0.85"/>')
                svg.append(text_svg((x1 + x2) / 2, ty, text))
                st = ("html=1;verticalAlign=bottom;labelBackgroundColor=none;fontFamily=Courier New;fontSize=11;edgeStyle=none;"
                      + ("dashed=1;endArrow=open;endFill=0;" if dashed else "endArrow=block;endFill=1;"))
                cells.append(
                    f'<mxCell id="{new_id()}" value={quoteattr(html(text))} style="{st}" edge="1" parent="1">'
                    f'<mxGeometry relative="1" as="geometry"><mxPoint x="{x1}" y="{y}" as="sourcePoint"/>'
                    f'<mxPoint x="{x2}" y="{y}" as="targetPoint"/></mxGeometry></mxCell>'
                )
                y += 30
            elif kind == "self":
                _, who, text, style = it
                dashed = style == "async"
                lines = text.split("\n")
                x = xs[idx[who]]
                h = max(18, len(lines) * LINE_H)
                dash = ' stroke-dasharray="6,4"' if dashed else ""
                marker = "url(#open)" if dashed else "url(#filled)"
                svg.append(f'<path d="M{x},{y} h30 v{h} h-30" fill="none" stroke="#222" stroke-width="1.3"{dash} marker-end="{marker}"/>')
                svg.append(text_svg(x + 36, y + 11, text, anchor="start"))
                cells.append(
                    f'<mxCell id="{new_id()}" style="html=1;edgeStyle=none;rounded=0;'
                    + ('dashed=1;endArrow=open;endFill=0;' if dashed else 'endArrow=block;endFill=1;')
                    + '" edge="1" parent="1">'
                    f'<mxGeometry relative="1" as="geometry"><mxPoint x="{x}" y="{y}" as="sourcePoint"/>'
                    f'<mxPoint x="{x}" y="{y + h}" as="targetPoint"/><Array as="points">'
                    f'<mxPoint x="{x + 30}" y="{y}"/><mxPoint x="{x + 30}" y="{y + h}"/></Array></mxGeometry></mxCell>'
                )
                tw = max(len(l) for l in lines) * CH_MONO + 10
                cells.append(
                    f'<mxCell id="{new_id()}" value={quoteattr(html(text))} style="text;html=1;align=left;verticalAlign=top;'
                    f'fontFamily=Courier New;fontSize=11;spacing=0;" vertex="1" parent="1">'
                    f'<mxGeometry x="{x + 36}" y="{y}" width="{tw}" height="{len(lines) * LINE_H}" as="geometry"/></mxCell>'
                )
                y += h + 30
            elif kind == "note":
                _, a, b, text = it
                lines = text.split("\n")
                tw = max(len(l) for l in lines) * CH_SANS + 20
                xa, xb = xs[idx[a]], xs[idx[b]]
                if a == b:
                    nx, nw = xa - tw / 2, tw
                else:
                    nx, nw = min(xa, xb) - 40, abs(xb - xa) + 80
                    if nw < tw:
                        nx -= (tw - nw) / 2
                        nw = tw
                nh = len(lines) * LINE_H + 10
                svg.append(
                    f'<path d="M{nx},{y} h{nw - 10} l10,10 v{nh - 10} h{-nw} z" fill="#fff2cc" stroke="#d6b656"/>'
                )
                svg.append(text_svg(nx + 8, y + 16, text, anchor="start", mono=False))
                cells.append(
                    f'<mxCell id="{new_id()}" value={quoteattr(html(text))} style="shape=note;whiteSpace=wrap;html=1;size=10;'
                    f'fillColor=#fff2cc;strokeColor=#d6b656;align=left;verticalAlign=top;spacingLeft=6;fontSize=11;" vertex="1" parent="1">'
                    f'<mxGeometry x="{nx}" y="{y}" width="{nw}" height="{nh}" as="geometry"/></mxCell>'
                )
                y += nh + 30
            elif kind == "frag":
                _, fkind, guard = it
                depth = len(frag_stack)
                frag_stack.append({"kind": fkind, "guard": guard, "y": y, "x": frag_left + depth * 8,
                                   "w": frag_right - frag_left - depth * 16, "else": []})
                y += 40
            elif kind == "else":
                frag_stack[-1]["else"].append((y, it[1]))
                y += 42
            elif kind == "end":
                f = frag_stack.pop()
                fy, fx, fw = f["y"], f["x"], f["w"]
                fh = y - fy
                tag_w = len(f["kind"]) * 8 + 22
                svg.insert(0,
                    f'<rect x="{fx}" y="{fy}" width="{fw}" height="{fh}" fill="none" stroke="#666" stroke-width="1"/>'
                    f'<path d="M{fx},{fy} h{tag_w} v12 l-8,8 h{-(tag_w - 8)} z" fill="#f0f0f0" stroke="#666"/>'
                    + text_svg(fx + 6, fy + 14, f["kind"], anchor="start", mono=False, weight="bold")
                    + text_svg(fx + tag_w + 8, fy + 14, f"[{f['guard']}]", anchor="start", mono=False, color="#444")
                )
                cells.append(
                    f'<mxCell id="{new_id()}" value="{escape(f["kind"])}" style="shape=umlFrame;whiteSpace=wrap;html=1;'
                    f'width={tag_w};height=20;boundedLbl=1;verticalAlign=middle;align=left;spacingLeft=5;fontStyle=1;fillColor=none;" '
                    f'vertex="1" parent="1"><mxGeometry x="{fx}" y="{fy}" width="{fw}" height="{fh}" as="geometry"/></mxCell>'
                )
                cells.append(
                    f'<mxCell id="{new_id()}" value={quoteattr(html("[" + f["guard"] + "]"))} style="text;html=1;align=left;'
                    f'verticalAlign=middle;fontSize=11;fontColor=#444444;" vertex="1" parent="1">'
                    f'<mxGeometry x="{fx + tag_w + 6}" y="{fy}" width="{len(f["guard"]) * CH_SANS + 20}" height="20" as="geometry"/></mxCell>'
                )
                for ey, eg in f["else"]:
                    svg.append(f'<line x1="{fx}" y1="{ey}" x2="{fx + fw}" y2="{ey}" stroke="#666" stroke-dasharray="5,4"/>')
                    svg.append(text_svg(fx + 8, ey + 16, f"[{eg}]", anchor="start", mono=False, color="#444"))
                    cells.append(
                        f'<mxCell id="{new_id()}" style="endArrow=none;dashed=1;html=1;strokeColor=#666666;" edge="1" parent="1">'
                        f'<mxGeometry relative="1" as="geometry"><mxPoint x="{fx}" y="{ey}" as="sourcePoint"/>'
                        f'<mxPoint x="{fx + fw}" y="{ey}" as="targetPoint"/></mxGeometry></mxCell>'
                    )
                    cells.append(
                        f'<mxCell id="{new_id()}" value={quoteattr(html("[" + eg + "]"))} style="text;html=1;align=left;'
                        f'verticalAlign=middle;fontSize=11;fontColor=#444444;" vertex="1" parent="1">'
                        f'<mxGeometry x="{fx + 6}" y="{ey + 2}" width="{len(eg) * CH_SANS + 20}" height="20" as="geometry"/></mxCell>'
                    )
                y += 28
            elif kind == "gap":
                y += it[1]

        assert not frag_stack, f"unclosed fragment in {self.title}"
        bottom = y + 10
        height = bottom + 20

        # lifelines and heads (drawn first, under messages)
        heads = []
        for i, (key, label, pkind) in enumerate(self.parts):
            fill, stroke = KIND_COLORS[pkind]
            x, w = xs[i], head_w[i]
            heads.append(
                f'<line x1="{x}" y1="{TOP + HEAD_H}" x2="{x}" y2="{bottom}" stroke="#999" stroke-dasharray="4,4"/>'
                f'<rect x="{x - w / 2}" y="{TOP}" width="{w}" height="{HEAD_H}" rx="6" fill="{fill}" stroke="{stroke}" stroke-width="1.5"/>'
                + text_svg(x, TOP + HEAD_H / 2 + 4 - (label.count("\n")) * LINE_H / 2, label, mono=False, size=12, weight="bold")
            )
            cells.insert(0,
                f'<mxCell id="{new_id()}" value={quoteattr(html(label))} style="shape=umlLifeline;perimeter=lifelinePerimeter;'
                f'whiteSpace=wrap;html=1;container=0;collapsible=0;recursiveResize=0;size={HEAD_H};rounded=1;fontStyle=1;'
                f'fillColor={fill};strokeColor={stroke};" vertex="1" parent="1">'
                f'<mxGeometry x="{x - w / 2}" y="{TOP}" width="{w}" height="{bottom - TOP}" as="geometry"/></mxCell>'
            )

        model = (
            f'<mxfile host="seqdraw"><diagram id="d1" name={quoteattr(self.title)}>'
            f'<mxGraphModel dx="0" dy="0" grid="1" gridSize="10" guides="1" page="0" math="0" shadow="0">'
            f'<root><mxCell id="0"/><mxCell id="1" parent="0"/>{"".join(cells)}</root></mxGraphModel></diagram></mxfile>'
        )
        defs = (
            '<defs>'
            '<marker id="filled" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse">'
            '<path d="M0,0 L10,5 L0,10 z" fill="#222"/></marker>'
            '<marker id="open" viewBox="0 0 10 10" refX="10" refY="5" markerWidth="8" markerHeight="8" orient="auto-start-reverse">'
            '<path d="M0,0 L10,5 L0,10" fill="none" stroke="#222" stroke-width="1.5"/></marker>'
            '</defs>'
        )
        out = (
            f'<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="{width:.0f}" height="{height:.0f}" '
            f'viewBox="0 0 {width:.0f} {height:.0f}" content={quoteattr(model)}>'
            f'{defs}<rect x="0" y="0" width="100%" height="100%" fill="#ffffff"/>'
            f'{"".join(heads)}{"".join(svg)}</svg>'
        )
        with open(path, "w", encoding="utf-8") as f:
            f.write(out)
